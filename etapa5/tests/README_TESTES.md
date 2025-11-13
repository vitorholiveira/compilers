# Testes Exaustivos - Etapa 5: Geração de Código ILOC

Este documento lista todos os testes criados para validar a geração de código intermediário ILOC.

## Estatísticas
- **Total de testes**: 81
- **Testes originais**: 18
- **Testes adicionados**: 63

## Categorias de Testes

### 1. Expressões Aritméticas (15 testes)

#### Operações Básicas
- `utest_expr_addition.txt` - Adição simples
- `utest_expr_subtraction.txt` - Subtração simples
- `utest_expr_multiplication.txt` - Multiplicação simples
- `utest_expr_division.txt` - Divisão simples
- `utest_expr_arithmetic_all.txt` - Todas operações aritméticas
- `utest_expr_arithmetic_chain.txt` - Cadeia de operações aritméticas
- `utest_expr_arithmetic_order.txt` - Ordem de avaliação

#### Operações Unárias
- `utest_expr_unary_negation.txt` - Operador unário de negação (-)
- `utest_expr_unary_plus.txt` - Operador unário de mais (+)

#### Expressões Complexas
- `utest_expr_complex.txt` - Expressão complexa básica
- `utest_expr_nested_deep.txt` - Expressão profundamente aninhada
- `utest_expr_precedence.txt` - Precedência de operadores
- `utest_expr_parentheses.txt` - Parênteses e precedência

#### Otimizações e Casos Especiais
- `utest_expr_immediate_optimization.txt` - Otimização com constantes imediatas
- `utest_expr_constant_folding.txt` - Expressões com constantes
- `utest_expr_mixed_types.txt` - Mistura de constantes e variáveis
- `utest_expr_negative_constants.txt` - Constantes negativas
- `utest_expr_division_edge.txt` - Casos especiais de divisão
- `utest_expr_zero_one.txt` - Valores especiais (0 e 1)
- `utest_expr_large_numbers.txt` - Números grandes
- `utest_expr_single_operand.txt` - Expressões com operando único

### 2. Expressões Lógicas (6 testes)

- `utest_expr_logical_and.txt` - Operação lógica AND
- `utest_expr_logical_or.txt` - Operação lógica OR
- `utest_expr_logical_complex.txt` - Operações lógicas complexas
- `utest_expr_logical_nested.txt` - Operações lógicas aninhadas
- `utest_expr_logical_precedence.txt` - Precedência de operadores lógicos
- `utest_expr_logical_short_circuit.txt` - Tradução numérica (and/or)

### 3. Expressões de Comparação (3 testes)

- `utest_expr_comparison.txt` - Comparações básicas
- `utest_expr_comparison_all.txt` - Todas as comparações (<, <=, >, >=, ==, !=)
- `utest_expr_comparison_immediate.txt` - Comparações com constantes

### 4. Comandos de Atribuição (6 testes)

- `utest_assignment_simple.txt` - Atribuição simples
- `utest_assignment_local_var.txt` - Atribuição a variável local
- `utest_assignment_global_var.txt` - Atribuição a variável global
- `utest_assignment_multiple.txt` - Múltiplas atribuições sequenciais
- `utest_assignment_chain.txt` - Cadeia de atribuições
- `utest_assignment_expression_complex.txt` - Atribuição com expressão complexa

### 5. Endereçamento de Variáveis (9 testes)

#### Variáveis Locais
- `utest_addressing_local.txt` - Variável local simples
- `utest_addressing_multiple_locals.txt` - Múltiplas variáveis locais

#### Variáveis Globais
- `utest_addressing_global.txt` - Variável global simples
- `utest_addressing_multiple_globals.txt` - Múltiplas variáveis globais

#### Casos Mistos
- `utest_addressing_mixed.txt` - Variáveis locais e globais misturadas
- `utest_addressing_expression.txt` - Expressões com variáveis
- `utest_addressing_load_store.txt` - Múltiplas operações load/store
- `utest_addressing_offset_calculation.txt` - Cálculo de offsets sequencial
- `utest_addressing_complex.txt` - Caso complexo de endereçamento

### 6. Fluxo de Controle - IF (8 testes)

- `utest_control_if_simple.txt` - IF simples
- `utest_control_if_else.txt` - IF-ELSE básico
- `utest_control_if_nested.txt` - IFs aninhados
- `utest_control_if_else_nested.txt` - IF-ELSE aninhados
- `utest_control_if_else_if.txt` - IF-ELSE-IF encadeado
- `utest_control_if_else_complex.txt` - IF-ELSE complexo
- `utest_control_if_else_multiple.txt` - Múltiplos IF-ELSE sequenciais
- `utest_control_if_else_jump.txt` - Verificação de jumpI em IF-ELSE
- `utest_control_if_false.txt` - IF com condição falsa

### 7. Fluxo de Controle - WHILE (6 testes)

- `utest_control_while.txt` - WHILE básico
- `utest_control_while_nested.txt` - WHILEs aninhados
- `utest_control_while_if.txt` - WHILE com IF interno
- `utest_control_while_complex.txt` - WHILE complexo
- `utest_control_while_jump.txt` - Verificação de jumpI em WHILE
- `utest_control_while_false.txt` - WHILE com condição falsa inicialmente
- `utest_control_multiple_while.txt` - Múltiplos loops while sequenciais

### 8. Fluxo de Controle - Casos Gerais (3 testes)

- `utest_control_sequential.txt` - Sequência de comandos de controle
- `utest_control_complex_nested.txt` - Aninhamento complexo
- `utest_control_empty_body.txt` - Corpos vazios

### 9. Estrutura ILOC (4 testes)

- `utest_iloc_structure.txt` - Estrutura básica ILOC
- `utest_iloc_structure_labels.txt` - Rótulos corretos (L1, L2, L3...)
- `utest_iloc_structure_temporaries.txt` - Temporários corretos (r1, r2, r3...)
- `utest_labels_temporaries.txt` - Rótulos e temporários básicos
- `utest_labels_unique.txt` - Verificação de rótulos únicos
- `utest_temporaries_reuse.txt` - Uso de temporários em sequência

### 10. Testes de Integração (10 testes)

- `utest_integration_complex.txt` - Programa complexo completo
- `utest_integration_factorial.txt` - Cálculo de fatorial simplificado
- `utest_integration_sum.txt` - Soma de números
- `utest_integration_max.txt` - Encontrar máximo
- `utest_integration_min.txt` - Encontrar mínimo
- `utest_integration_swap.txt` - Troca de valores
- `utest_integration_counter.txt` - Contador com condição
- `utest_integration_power.txt` - Cálculo de potência simplificado
- `utest_integration_conditional_sum.txt` - Soma condicional
- `utest_integration_fibonacci.txt` - Sequência de Fibonacci simplificada

### 11. Testes Abrangentes (1 teste)

- `utest_expr_all_operators.txt` - Todos os operadores em um programa

## Como Executar os Testes

### Executar Todos os Testes Automaticamente

**Opção 1: Script completo (recomendado)**
```bash
./run_tests.sh
```

**Opção 2: Script completo com opções**
```bash
./run_tests.sh -v          # Modo verbose (mostra detalhes)
./run_tests.sh -q          # Modo quiet (apenas resumo)
./run_tests.sh -o          # Salva saída ILOC de cada teste
./run_tests.sh --help      # Mostra ajuda
```

**Opção 3: Script simples**
```bash
./run_tests_simple.sh
```

### Executar um Teste Individual

```bash
./etapa5 < tests/utest_nome_do_teste.txt > saida.iloc
python3 ilocsim.py < saida.iloc
```

Ou diretamente:
```bash
./etapa5 < tests/utest_nome_do_teste.txt | python3 ilocsim.py
```

### Executar Testes Manualmente (sem script)

```bash
for test in tests/utest_*.txt; do
    echo "Testando: $test"
    ./etapa5 < "$test" | python3 ilocsim.py
done
```

## Cobertura de Funcionalidades

### ✅ Expressões
- [x] Operações aritméticas: +, -, *, /
- [x] Operações lógicas: &&, ||
- [x] Operações de comparação: <, <=, >, >=, ==, !=
- [x] Operadores unários: +, -
- [x] Precedência de operadores
- [x] Parênteses
- [x] Expressões aninhadas
- [x] Constantes e variáveis misturadas

### ✅ Atribuições
- [x] Atribuição simples
- [x] Atribuição a variáveis locais
- [x] Atribuição a variáveis globais
- [x] Múltiplas atribuições
- [x] Atribuições com expressões complexas

### ✅ Endereçamento
- [x] Variáveis locais (rfp + offset)
- [x] Variáveis globais (rbss + offset)
- [x] Cálculo de offsets sequencial
- [x] Múltiplas variáveis locais
- [x] Múltiplas variáveis globais
- [x] Mistura de locais e globais

### ✅ Fluxo de Controle
- [x] IF simples
- [x] IF-ELSE
- [x] IF-ELSE aninhados
- [x] WHILE básico
- [x] WHILE aninhados
- [x] WHILE com IF interno
- [x] Múltiplos comandos de controle

### ✅ Estrutura ILOC
- [x] Rótulos (L1, L2, L3...)
- [x] Temporários (r1, r2, r3...)
- [x] Operações de três endereços
- [x] Operações de fluxo de controle (cbr, jumpI)
- [x] Operações de memória (loadAI, storeAI)
- [x] Operações imediatas (addI, subI, multI, divI)

## Notas Importantes

1. **Convenções ILOC**: Todos os testes seguem as convenções especificadas:
   - Rótulos: L1, L2, L3... (sempre começam com L maiúsculo)
   - Temporários: r1, r2, r3... (sempre começam com r minúsculo)
   - Registradores reservados: rfp, rsp, rbss, rpc (não usados como temporários)

2. **Endereçamento**: 
   - Variáveis locais: offset negativo em relação a rfp
   - Variáveis globais: offset positivo em relação a rbss
   - Assumindo que int ocupa 4 bytes

3. **Expressões Lógicas**: Usam tradução numérica (and/or) ao invés de controle de fluxo, conforme especificação.

4. **Validação**: Os testes devem ser validados usando o simulador `ilocsim.py` fornecido, verificando o estado final da memória.

