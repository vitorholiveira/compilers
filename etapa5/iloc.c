#include "iloc.h"
#include <string.h>

/* Variáveis globais para contadores */
int register_counter = 0;
int label_counter = 0;

/* ============================================================================
 * FUNÇÕES AUXILIARES PARA CONTADORES
 * ============================================================================ */

int get_next_register(void) {
    return ++register_counter;
}

int get_next_label(void) {
    return ++label_counter;
}

/* ============================================================================
 * FUNÇÕES PARA CRIAÇÃO DE OPERANDOS
 * ============================================================================ */

iloc_operand_t* iloc_operand_new_reg(void) {
    iloc_operand_t* operand = (iloc_operand_t*)malloc(sizeof(iloc_operand_t));
    if (!operand) {
        return NULL;
    }
    
    operand->type = REGISTER;
    int reg_num = get_next_register();
    
    // Alocar espaço para string "r" + número (máximo 10 dígitos)
    operand->value.str_value = (char*)malloc(12 * sizeof(char));
    if (!operand->value.str_value) {
        free(operand);
        return NULL;
    }
    // Debug
    // sprintf(operand->value.str_value, "r%d", reg_num);
    return operand;
}

iloc_operand_t* iloc_operand_new_const(int value) {
    iloc_operand_t* operand = (iloc_operand_t*)malloc(sizeof(iloc_operand_t));
    if (!operand) {
        return NULL;
    }
    
    operand->type = CONSTANT;
    operand->value.int_value = value;
    return operand;
}

iloc_operand_t* iloc_operand_new_label(void) {
    iloc_operand_t* operand = (iloc_operand_t*)malloc(sizeof(iloc_operand_t));
    if (!operand) {
        return NULL;
    }
    
    operand->type = LABEL;
    int label_num = get_next_label();
    
    // Alocar espaço para string "L" + número (máximo 10 dígitos)
    operand->value.str_value = (char*)malloc(12 * sizeof(char));
    if (!operand->value.str_value) {
        free(operand);
        return NULL;
    }
    // Debug
    // sprintf(operand->value.str_value, "L%d", label_num);
    return operand;
}

/* ============================================================================
 * FUNÇÕES PARA LIBERAÇÃO DE OPERANDOS
 * ============================================================================ */

void iloc_operand_free(iloc_operand_t* operand) {
    if (!operand) {
        return;
    }
    
    // Se for registrador ou rótulo, liberar a string
    if (operand->type == REGISTER || operand->type == LABEL) {
        if (operand->value.str_value) {
            free(operand->value.str_value);
        }
    }
    
    free(operand);
}

/* ============================================================================
 * FUNÇÕES PARA CRIAÇÃO E MANIPULAÇÃO DE OPERAÇÕES
 * ============================================================================ */

iloc_operation_t* iloc_operation_new(const char* opcode, bool is_fluxo_controle) {
    iloc_operation_t* op = (iloc_operation_t*)malloc(sizeof(iloc_operation_t));
    if (!op) {
        return NULL;
    }
    
    // Copiar opcode
    op->opcode = (char*)malloc((strlen(opcode) + 1) * sizeof(char));
    if (!op->opcode) {
        free(op);
        return NULL;
    }
    strcpy(op->opcode, opcode);
    
    op->is_fluxo_controle = is_fluxo_controle;
    op->source_operands = NULL;
    op->target_operands = NULL;
    op->num_source_operands = 0;
    op->num_target_operands = 0;
    op->next = NULL;
    
    return op;
}

void iloc_operation_add_source(iloc_operation_t* op, iloc_operand_t* operand) {
    if (!op || !operand) {
        return;
    }
    
    // Realocar array de operandos fonte
    int new_size = op->num_source_operands + 1;
    iloc_operand_t** new_array = (iloc_operand_t**)realloc(
        op->source_operands, 
        new_size * sizeof(iloc_operand_t*)
    );
    
    if (!new_array) {
        return; // Falha na alocação
    }
    
    op->source_operands = new_array;
    op->source_operands[op->num_source_operands] = operand;
    op->num_source_operands = new_size;
}

void iloc_operation_add_target(iloc_operation_t* op, iloc_operand_t* operand) {
    if (!op || !operand) {
        return;
    }
    
    // Realocar array de operandos alvo
    int new_size = op->num_target_operands + 1;
    iloc_operand_t** new_array = (iloc_operand_t**)realloc(
        op->target_operands, 
        new_size * sizeof(iloc_operand_t*)
    );
    
    if (!new_array) {
        return; // Falha na alocação
    }
    
    op->target_operands = new_array;
    op->target_operands[op->num_target_operands] = operand;
    op->num_target_operands = new_size;
}

void iloc_operation_free(iloc_operation_t* op) {
    if (!op) {
        return;
    }
    
    // Liberar opcode
    if (op->opcode) {
        free(op->opcode);
    }
    
    // Liberar operandos fonte
    if (op->source_operands) {
        // Nota: Não liberamos os operandos aqui porque podem ser compartilhados
        // A liberação dos operandos deve ser feita separadamente
        free(op->source_operands);
    }
    
    // Liberar operandos alvo
    if (op->target_operands) {
        // Nota: Não liberamos os operandos aqui porque podem ser compartilhados
        // A liberação dos operandos deve ser feita separadamente
        free(op->target_operands);
    }
    
    free(op);
}

/* ============================================================================
 * FUNÇÕES PARA CRIAÇÃO E MANIPULAÇÃO DE CÓDIGO ILOC
 * ============================================================================ */

iloc_code_t* iloc_code_new(void) {
    iloc_code_t* code = (iloc_code_t*)malloc(sizeof(iloc_code_t));
    if (!code) {
        return NULL;
    }
    
    code->first = NULL;
    code->last = NULL;
    code->count = 0;
    
    return code;
}

void iloc_code_append(iloc_code_t* code, iloc_operation_t* op) {
    if (!code || !op) {
        return;
    }
    
    // Se a lista está vazia
    if (code->count == 0) {
        code->first = op;
        code->last = op;
    } else {
        // Adicionar ao final da lista
        code->last->next = op;
        code->last = op;
    }
    
    op->next = NULL; // Garantir que é o último
    code->count++;
}

void iloc_code_concat(iloc_code_t* dest, iloc_code_t* src) {
    if (!dest || !src || src->count == 0) {
        return;
    }
    
    // Se dest está vazio, apenas copiar src
    if (dest->count == 0) {
        dest->first = src->first;
        dest->last = src->last;
        dest->count = src->count;
    } else {
        // Concatenar src ao final de dest
        dest->last->next = src->first;
        dest->last = src->last;
        dest->count += src->count;
    }
    
    // Limpar src (mas não liberar as operações, elas agora pertencem a dest)
    src->first = NULL;
    src->last = NULL;
    src->count = 0;
}

void iloc_code_free(iloc_code_t* code) {
    if (!code) {
        return;
    }
    
    // Liberar todas as operações da lista
    iloc_operation_t* current = code->first;
    while (current) {
        iloc_operation_t* next = current->next;
        iloc_operation_free(current);
        current = next;
    }
    
    free(code);
}

