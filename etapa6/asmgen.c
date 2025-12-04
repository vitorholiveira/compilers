#include "asmgen.h"
#include "table.h"
#include "iloc.h"
#include <string.h>
#include <ctype.h>

/* ============================================================================
 * Funções auxiliares para acesso à tabela de símbolos global
 * ============================================================================ */

/* Retorna a tabela de símbolos do escopo global (bottom da pilha). */
static table_t* get_global_table(stack_t* scopes) {
    if (!scopes) {
        return NULL;
    }

    /* Caso usual: ainda há escopos ativos na pilha (usa 'top' e desce até o bottom). */
    scope_node_t* bottom = scopes->top;
    while (bottom && bottom->below) {
        bottom = bottom->below;
    }
    if (bottom && bottom->table) {
        return bottom->table;
    }

    /* Após o parsing, o escopo global pode ter sido movido para 'archived'.
     * Nessa lista, o escopo global é o MAIS RECENTE empilhado/desempilhado,
     * portanto o primeiro nó da lista 'archived'. */
    if (scopes->archived && scopes->archived->table) {
        return scopes->archived->table;
    }

    return NULL;
}

/* ============================================================================
 * Geração do segmento de dados (.data)
 * ============================================================================ */

/* Gera um bloco contínuo de memória para variáveis globais, compatível com
 * o esquema de offsets usado na etapa 5:
 *
 *   - cada variável global ocupa 4 bytes;
 *   - o campo symbol->offset armazena o deslocamento (0, 4, 8, ...) em bytes;
 *   - o ILOC acessa globais via registrador base "rbss" e esses offsets.
 *
 * No assembly, representamos esse bloco como:
 *
 *   .data
 *   .align 4
 * rbss_data:
 *   .zero <tamanho_total_em_bytes>
 *
 * Retorna 1 se o rótulo rbss_data foi emitido (há globais), 0 caso contrário.
 * Em etapas posteriores, um registrador físico (por exemplo, %rbx) poderá
 * ser configurado para apontar para rbss_data, implementando o papel de "rbss".
 */
static int asmgen_emit_data_segment(stack_t* scopes, FILE* out) {
    table_t* global_table = get_global_table(scopes);
    if (!global_table) {
        return 0;
    }

    /* Descobrir o maior offset dentre as variáveis globais. */
    int max_offset = -1;
    symbol_t* sym = global_table->head;
    while (sym) {
        if (sym->nature == IDENTIFIER) {
            if (sym->offset > max_offset) {
                max_offset = sym->offset;
            }
        }
        sym = sym->next;
    }

    /* Se não há variáveis globais, não há o que emitir em .data. */
    if (max_offset < 0) {
        return 0;
    }

    /* Como cada variável ocupa 4 bytes, o tamanho total é max_offset + 4. */
    int total_bytes = max_offset + 4;

    fprintf(out, "\t.data\n");
    fprintf(out, "\t.align 4\n");
    fprintf(out, "rbss_data:\n");
    fprintf(out, "\t.zero %d\n\n", total_bytes);

    return 1;
}

/* ============================================================================
 * Geração do segmento de código (.text) e função main
 * ============================================================================ */

static void asmgen_emit_text_header(FILE* out) {
    fprintf(out, "\t.text\n");
    fprintf(out, "\t.globl main\n");
    fprintf(out, "\t.type main, @function\n");
}

static void asmgen_emit_main_prologue(FILE* out, int has_globals) {
    fprintf(out, "main:\n");
    fprintf(out, "\tpushq\t%%rbp\n");
    fprintf(out, "\tmovq\t%%rsp, %%rbp\n");

    /* Inicializar registrador base para dados globais (rbss -> %rbx) somente se existirem globais */
    if (has_globals) {
        fprintf(out, "\tleaq\trbss_data(%%rip), %%rbx\n");
    }
}

static void asmgen_emit_main_epilogue(FILE* out) {
    fprintf(out, "\tleave\n");
    fprintf(out, "\tret\n");
}

/* ============================================================================
 * Coleta de código ILOC a partir da AST
 * ============================================================================ */

/* Retorna 1 se o rótulo corresponde a um nó interno (expressão/comando),
   cujo iloc_code não deve ser concatenado no código final do programa. */
static int is_internal_label(const char* label) {
    if (!label) return 0;

    /* Operadores aritméticos e lógicos e nós internos da linguagem */
    const char* internal_ops[] = {
        ":=", "se", "enquanto", "retorna", "seq", "bloco_vazio",
        "+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=",
        "&", "&&", "|", "||", "!", NULL
    };

    for (int i = 0; internal_ops[i] != NULL; i++) {
        if (strcmp(label, internal_ops[i]) == 0) {
            return 1;
        }
    }

    /* Nós de chamada de função ("call f") também são internos */
    if (strncmp(label, "call ", 5) == 0) {
        return 1;
    }

    return 0;
}

/* Percorre a AST e concatena apenas os códigos ILOC de nós "de topo"
   (tipicamente definições de função). Nós internos de expressão/comando
   são ignorados aqui, pois seu código já foi incorporado ao corpo
   das funções correspondentes. */
static void collect_iloc(asd_tree_t* node, iloc_code_t* acc) {
    if (!node || !acc) return;

    if (node->iloc_code && node->iloc_code->count > 0 && !is_internal_label(node->label)) {
        iloc_code_concat(acc, node->iloc_code);
    }

    for (int i = 0; i < node->number_of_children; i++) {
        collect_iloc(node->children[i], acc);
    }
}

/* ============================================================================
 * Tradução de ILOC para assembly x86_64
 * ============================================================================ */

/* Tamanho (em bytes) reservado para variáveis locais abaixo de %rbp.
 * Usado para posicionar temporários depois da região de locais. */
static int current_locals_size = 0;

/* Extrai o índice numérico de um registrador temporário "rN".
 * Retorna -1 se não for um temporário (por exemplo, "rfp" ou "rbss"). */
static int get_temp_index(iloc_operand_t* op) {
    if (!op || op->type != REGISTER || !op->value.str_value) {
        return -1;
    }
    const char* s = op->value.str_value;
    if (s[0] != 'r' || !isdigit((unsigned char)s[1])) {
        return -1;
    }
    return atoi(s + 1);
}

/* Retorna o maior índice de registrador temporário usado em 'code'. */
static int get_max_temp_index(iloc_code_t* code) {
    int max_idx = -1;
    if (!code) return -1;

    iloc_operation_t* op = code->first;
    while (op) {
        for (int i = 0; i < op->num_source_operands; i++) {
            int idx = get_temp_index(op->source_operands[i]);
            if (idx > max_idx) max_idx = idx;
        }
        for (int i = 0; i < op->num_target_operands; i++) {
            int idx = get_temp_index(op->target_operands[i]);
            if (idx > max_idx) max_idx = idx;
        }
        op = op->next;
    }
    return max_idx;
}

/* Retorna o maior offset de variável local (base rfp) usado em 'code'. */
static int get_max_local_offset(iloc_code_t* code) {
    int max_off = -1;
    if (!code) return -1;

    iloc_operation_t* op = code->first;
    while (op) {
        if (strcmp(op->opcode, "loadAI") == 0 &&
            op->num_source_operands >= 2) {
            iloc_operand_t* base = op->source_operands[0];
            iloc_operand_t* off_op = op->source_operands[1];
            if (base && base->type == REGISTER && base->value.str_value &&
                strcmp(base->value.str_value, "rfp") == 0) {
                int off = off_op->value.int_value;
                if (off > max_off) max_off = off;
            }
        } else if (strcmp(op->opcode, "storeAI") == 0 &&
                   op->num_target_operands >= 2) {
            iloc_operand_t* base = op->target_operands[0];
            iloc_operand_t* off_op = op->target_operands[1];
            if (base && base->type == REGISTER && base->value.str_value &&
                strcmp(base->value.str_value, "rfp") == 0) {
                int off = off_op->value.int_value;
                if (off > max_off) max_off = off;
            }
        }
        op = op->next;
    }
    return max_off;
}

/* Offset em bytes na pilha para um temporário rN.
 * Temporários ficam DEPOIS da região de variáveis locais:
 *   locals: offsets -4, -8, ..., -(current_locals_size)
 *   temps:  abaixo disso.
 */
static int temp_offset_bytes(int temp_index) {
    return -(current_locals_size + 4 * (temp_index + 1));
}

/* Helpers para carregar/armazenar temporários. */
static void emit_load_temp(FILE* out, const char* reg, int temp_index) {
    if (!out || temp_index < 0) return;
    int offset = temp_offset_bytes(temp_index);
    fprintf(out, "\tmovl\t%d(%%rbp), %s\n", offset, reg);
}

static void emit_store_temp(FILE* out, const char* reg, int temp_index) {
    if (!out || temp_index < 0) return;
    int offset = temp_offset_bytes(temp_index);
    fprintf(out, "\tmovl\t%s, %d(%%rbp)\n", reg, offset);
}

/* Converte registradores base especiais do ILOC para registradores físicos. */
static const char* asm_base_reg_for_iloc(const char* iloc_reg) {
    if (!iloc_reg) return "%rbp";
    if (strcmp(iloc_reg, "rfp") == 0) {
        return "%rbp";
    }
    if (strcmp(iloc_reg, "rbss") == 0) {
        return "%rbx";
    }
    /* Por padrão, tratamos como %rbp. */
    return "%rbp";
}

static void asmgen_translate_iloc_code(iloc_code_t* code, FILE* out) {
    if (!code || !out || code->count == 0) {
        return;
    }

    /* Reservar espaço na pilha para variáveis locais (base rfp) e temporários rN. */
    int max_temp = get_max_temp_index(code);
    int max_local = get_max_local_offset(code);

    int locals_size = (max_local >= 0) ? (max_local + 4) : 0;
    int temps_size  = (max_temp >= 0) ? 4 * (max_temp + 1) : 0;
    int total_bytes = locals_size + temps_size;

    current_locals_size = locals_size;

    if (total_bytes > 0) {
        fprintf(out, "\tsubq\t$%d, %%rsp\n", total_bytes);
    }
    /* Rastrear o último registrador temporário definido (rN).
     * Usaremos o valor de rN como valor de retorno da função. */
    int last_temp = -1;

    iloc_operation_t* op = code->first;
    while (op) {
        /* Emitir rótulo, se houver. */
        if (op->label && op->label->type == LABEL && op->label->value.str_value) {
            fprintf(out, "%s:\n", op->label->value.str_value);
        }

        const char* opcode = op->opcode;

        if (strcmp(opcode, "nop") == 0) {
            /* nada a fazer */
        }
        else if (strcmp(opcode, "loadI") == 0 && op->num_source_operands >= 1 && op->num_target_operands >= 1) {
            int value = op->source_operands[0]->value.int_value;
            int dst = get_temp_index(op->target_operands[0]);
            fprintf(out, "\tmovl\t$%d, %%eax\n", value);
            emit_store_temp(out, "%eax", dst);
        }
        else if (strcmp(opcode, "i2i") == 0 && op->num_source_operands >= 1 && op->num_target_operands >= 1) {
            int src = get_temp_index(op->source_operands[0]);
            int dst = get_temp_index(op->target_operands[0]);
            emit_load_temp(out, "%eax", src);
            emit_store_temp(out, "%eax", dst);
        }
        else if ((strcmp(opcode, "add") == 0 || strcmp(opcode, "sub") == 0 ||
                  strcmp(opcode, "mult") == 0 || strcmp(opcode, "div") == 0) &&
                 op->num_source_operands >= 2 && op->num_target_operands >= 1) {
            int a = get_temp_index(op->source_operands[0]);
            int b = get_temp_index(op->source_operands[1]);
            int dst = get_temp_index(op->target_operands[0]);
            emit_load_temp(out, "%eax", a);
            emit_load_temp(out, "%ecx", b);

            if (strcmp(opcode, "add") == 0) {
                fprintf(out, "\taddl\t%%ecx, %%eax\n");
            } else if (strcmp(opcode, "sub") == 0) {
                fprintf(out, "\tsubl\t%%ecx, %%eax\n");
            } else if (strcmp(opcode, "mult") == 0) {
                fprintf(out, "\timull\t%%ecx, %%eax\n");
            } else if (strcmp(opcode, "div") == 0) {
                fprintf(out, "\tcltd\n");
                fprintf(out, "\tidivl\t%%ecx\n");
            }

            emit_store_temp(out, "%eax", dst);
        }
        else if (strcmp(opcode, "rsubI") == 0 && op->num_source_operands >= 2 && op->num_target_operands >= 1) {
            int src = get_temp_index(op->source_operands[0]);
            int imm = op->source_operands[1]->value.int_value;
            int dst = get_temp_index(op->target_operands[0]);
            emit_load_temp(out, "%eax", src);
            fprintf(out, "\tmovl\t$%d, %%ecx\n", imm);
            fprintf(out, "\tsubl\t%%eax, %%ecx\n");
            emit_store_temp(out, "%ecx", dst);
        }
        else if (strcmp(opcode, "xorI") == 0 && op->num_source_operands >= 2 && op->num_target_operands >= 1) {
            int src = get_temp_index(op->source_operands[0]);
            int imm = op->source_operands[1]->value.int_value;
            int dst = get_temp_index(op->target_operands[0]);
            emit_load_temp(out, "%eax", src);
            fprintf(out, "\tmovl\t$%d, %%ecx\n", imm);
            fprintf(out, "\txorl\t%%ecx, %%eax\n");
            emit_store_temp(out, "%eax", dst);
        }
        else if ((strcmp(opcode, "and") == 0 || strcmp(opcode, "or") == 0) &&
                 op->num_source_operands >= 2 && op->num_target_operands >= 1) {
            int a = get_temp_index(op->source_operands[0]);
            int b = get_temp_index(op->source_operands[1]);
            int dst = get_temp_index(op->target_operands[0]);
            emit_load_temp(out, "%eax", a);
            emit_load_temp(out, "%ecx", b);
            if (strcmp(opcode, "and") == 0) {
                fprintf(out, "\tandl\t%%ecx, %%eax\n");
            } else {
                fprintf(out, "\torl\t%%ecx, %%eax\n");
            }
            emit_store_temp(out, "%eax", dst);
        }
        else if (strncmp(opcode, "cmp_", 4) == 0 &&
                 op->num_source_operands >= 2 && op->num_target_operands >= 1) {
            int a = get_temp_index(op->source_operands[0]);
            int b = get_temp_index(op->source_operands[1]);
            int dst = get_temp_index(op->target_operands[0]);
            emit_load_temp(out, "%eax", a);
            emit_load_temp(out, "%ecx", b);
            fprintf(out, "\tcmpl\t%%ecx, %%eax\n");

            const char* cond = opcode + 4; /* LT, LE, GT, GE, EQ, NE */
            if (strcmp(cond, "LT") == 0) {
                fprintf(out, "\tsetl\t%%al\n");
            } else if (strcmp(cond, "LE") == 0) {
                fprintf(out, "\tsetle\t%%al\n");
            } else if (strcmp(cond, "GT") == 0) {
                fprintf(out, "\tsetg\t%%al\n");
            } else if (strcmp(cond, "GE") == 0) {
                fprintf(out, "\tsetge\t%%al\n");
            } else if (strcmp(cond, "EQ") == 0) {
                fprintf(out, "\tsete\t%%al\n");
            } else if (strcmp(cond, "NE") == 0) {
                fprintf(out, "\tsetne\t%%al\n");
            }
            fprintf(out, "\tmovzbl\t%%al, %%eax\n");
            emit_store_temp(out, "%eax", dst);
        }
        else if (strcmp(opcode, "loadAI") == 0 &&
                 op->num_source_operands >= 2 && op->num_target_operands >= 1) {
            iloc_operand_t* base = op->source_operands[0];
            iloc_operand_t* offset_op = op->source_operands[1];
            int dst = get_temp_index(op->target_operands[0]);
            const char* base_name = base->value.str_value;
            const char* base_reg = asm_base_reg_for_iloc(base_name);
            int offset = offset_op->value.int_value;

            int phys_off = offset;
            if (strcmp(base_name, "rfp") == 0) {
                /* Variáveis locais: offset 0,4,8,... em ILOC ⇒ -4,-8,... em x86 */
                phys_off = -(offset + 4);
            }

            fprintf(out, "\tmovl\t%d(%s), %%eax\n", phys_off, base_reg);
            emit_store_temp(out, "%eax", dst);
        }
        else if (strcmp(opcode, "storeAI") == 0 &&
                 op->num_source_operands >= 1 && op->num_target_operands >= 2) {
            int src = get_temp_index(op->source_operands[0]);
            iloc_operand_t* base = op->target_operands[0];
            iloc_operand_t* offset_op = op->target_operands[1];

            const char* base_name = base->value.str_value;
            const char* base_reg = asm_base_reg_for_iloc(base_name);
            int offset = offset_op->value.int_value;

            int phys_off = offset;
            if (strcmp(base_name, "rfp") == 0) {
                phys_off = -(offset + 4);
            }

            emit_load_temp(out, "%eax", src);
            fprintf(out, "\tmovl\t%%eax, %d(%s)\n", phys_off, base_reg);
        }
        else if (strcmp(opcode, "cbr") == 0 &&
                 op->num_source_operands >= 1 && op->num_target_operands >= 2) {
            int cond = get_temp_index(op->source_operands[0]);
            iloc_operand_t* l_true = op->target_operands[0];
            iloc_operand_t* l_false = op->target_operands[1];

            emit_load_temp(out, "%eax", cond);
            fprintf(out, "\tcmpl\t$0, %%eax\n");
            fprintf(out, "\tjne\t%s\n", l_true->value.str_value);
            fprintf(out, "\tjmp\t%s\n", l_false->value.str_value);
        }
        else if (strcmp(opcode, "jumpI") == 0 && op->num_target_operands >= 1) {
            iloc_operand_t* target = op->target_operands[0];
            fprintf(out, "\tjmp\t%s\n", target->value.str_value);
        }

        /* Atualizar último temporário definido (se houver). */
        for (int i = 0; i < op->num_target_operands; i++) {
            int idx = get_temp_index(op->target_operands[i]);
            if (idx >= 0) {
                last_temp = idx;
            }
        }

        op = op->next;
    }

    /* Definir valor de retorno de main em %eax.
     * Convencão adotada: o valor de retorno é o último temporário rN
     * definido pelo código ILOC (assim como usado nos testes de ILOC). */
    if (last_temp >= 0) {
        emit_load_temp(out, "%eax", last_temp);
    } else {
        fprintf(out, "\tmovl\t$0, %%eax\n");
    }
}

/* ============================================================================
 * Ponto de entrada da geração de assembly
 * ============================================================================ */

void generate_assembly_program(asd_tree_t* program_root, stack_t* scopes, FILE* out) {
    if (!program_root || !scopes || !out) {
        return;
    }

    /* 1) Segmento de dados (.data) com as variáveis globais. */
    int has_globals = asmgen_emit_data_segment(scopes, out);

    /* 2) Coletar código ILOC de alto nível do programa. */
    iloc_code_t* program_code = iloc_code_new();
    if (!program_code) {
        return;
    }
    collect_iloc(program_root, program_code);

    /* 3) Segmento de código (.text) com função main.
     *    Em breve, o corpo será preenchido com a tradução de program_code. */
    asmgen_emit_text_header(out);
    asmgen_emit_main_prologue(out, has_globals);
    asmgen_translate_iloc_code(program_code, out);
    asmgen_emit_main_epilogue(out);

    iloc_code_free(program_code);
}

