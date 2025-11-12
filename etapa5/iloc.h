#ifndef ILOC_H
#define ILOC_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* Tipo de operando ILOC */
typedef enum {REGISTER = 0, CONSTANT = 1, LABEL = 2} operand_type_t;

/* Estrutura para representar um operando ILOC */
typedef struct iloc_operand {
    operand_type_t type;
    union {
        char* str_value;  // Para registradores (ex: "r1") e rótulos (ex: "L1")
        int int_value;    // Para constantes numéricas
    } value;
} iloc_operand_t;

/* Estrutura para representar uma operação ILOC */
typedef struct iloc_operation {
    char* opcode;                          // Código da operação (ex: "add", "loadI", "cbr")
    iloc_operand_t* label;                 // Rótulo associado à operação (NULL se não tem rótulo)
    iloc_operand_t** source_operands;      // Lista de operandos fonte
    int num_source_operands;               // Número de operandos fonte
    iloc_operand_t** target_operands;     // Lista de operandos alvo
    int num_target_operands;               // Número de operandos alvo
    bool is_fluxo_controle;                // true se usa '->', false se usa '=>'
    struct iloc_operation* next;           // Próxima operação na lista (para lista encadeada)
} iloc_operation_t;

/* Estrutura para representar uma lista de operações ILOC (código completo) */
typedef struct iloc_code {
    iloc_operation_t* first;               // Primeira operação da lista
    iloc_operation_t* last;                // Última operação da lista (para inserção eficiente)
    int count;                             // Número de operações
} iloc_code_t;

/* Declarações de variáveis globais (definidas no .c) */
extern int register_counter;
extern int label_counter;

/* Funções para criação de operandos */
iloc_operand_t* iloc_operand_new_reg(void);           // Retorna r{register_counter++}
iloc_operand_t* iloc_operand_new_const(int value);    // Retorna constante numérica
iloc_operand_t* iloc_operand_new_label(void);         // Retorna L{label_counter++}

/* Funções para criação e manipulação de operações */
iloc_operation_t* iloc_operation_new(const char* opcode, bool is_fluxo_controle);
void iloc_operation_set_label(iloc_operation_t* op, iloc_operand_t* label);
void iloc_operation_add_source(iloc_operation_t* op, iloc_operand_t* operand);
void iloc_operation_add_target(iloc_operation_t* op, iloc_operand_t* operand);
void iloc_operation_free(iloc_operation_t* op);

/* Funções para criação e manipulação de código ILOC */
iloc_code_t* iloc_code_new(void);
void iloc_code_append(iloc_code_t* code, iloc_operation_t* op);
void iloc_code_concat(iloc_code_t* dest, iloc_code_t* src);
void iloc_code_free(iloc_code_t* code);

/* Funções para liberação de operandos */
void iloc_operand_free(iloc_operand_t* operand);

/* Funções auxiliares para contadores */
int get_next_register(void);  // Incrementa e retorna próximo número de registrador
int get_next_label(void);     // Incrementa e retorna próximo número de rótulo

/* ============================================================================
 * FUNÇÕES DE IMPRESSÃO DO CÓDIGO ILOC
 * ============================================================================ */

/* Passo 9.1: Imprime um operando ILOC */
void iloc_print_operand(iloc_operand_t* op, FILE* out);

/* Passo 9.2: Imprime uma operação ILOC */
void iloc_print_operation(iloc_operation_t* op, FILE* out);

/* Passo 9.3: Imprime código ILOC completo */
void iloc_print_code(iloc_code_t* code, FILE* out);

#endif // ILOC_H
