# Roadmap - Etapa 5: Geração de Código ILOC

## Visão Geral
Este documento apresenta um plano passo-a-passo para implementar a geração de código intermediário ILOC a partir da AST já construída nas etapas anteriores.

---

## FASE 1: Estruturas de Dados Fundamentais

### Passo 1.1: Criar arquivo `iloc.h`
- Criar arquivo de cabeçalho para estruturas e funções ILOC
- Definir estruturas básicas para operações ILOC
- Planejar a estrutura de dados para representar uma operação ILOC:
  - Opcode (string)
  - Lista de operandos fonte (podem ser registradores, constantes ou rótulos)
  - Lista de operandos alvo (podem ser registradores, constantes ou rótulos)
  - Flag para indicar se é operação de fluxo de controle (usa `->` ao invés de `=>`)

### Passo 1.2: Definir estrutura para operando ILOC
- Criar enum ou union para representar os três tipos de operandos:
  - Registrador (ex: "r1", "r2")
  - Número/Constante (ex: 42, -10)
  - Rótulo (ex: "L1", "L2")
- Criar estrutura `iloc_operand_t` que pode representar qualquer um dos três tipos

### Passo 1.3: Definir estrutura para operação ILOC
- Criar estrutura `iloc_operation_t` contendo:
  - String do opcode (ex: "add", "loadI", "cbr")
  - Array/lista de operandos fonte
  - Array/lista de operandos alvo
  - Flag booleana indicando se é operação de fluxo de controle
  - Ponteiro para próxima operação (para lista encadeada)

### Passo 1.4: Definir estrutura para lista de operações ILOC
- Criar estrutura `iloc_code_t` que representa uma lista de operações ILOC
- Pode ser uma lista encadeada ou array dinâmico
- Deve ter funções para:
  - Adicionar operação ao final
  - Concatenar duas listas de código
  - Liberar memória

### Passo 1.5: Implementar funções básicas de manipulação
- `iloc_operand_new_reg(char* reg_name)` - cria operando do tipo registrador
- `iloc_operand_new_const(int value)` - cria operando do tipo constante
- `iloc_operand_new_label(char* label_name)` - cria operando do tipo rótulo
- `iloc_operation_new(char* opcode, ...)` - cria nova operação
- `iloc_code_new()` - cria lista vazia de código
- `iloc_code_append(iloc_code_t* code, iloc_operation_t* op)` - adiciona operação
- `iloc_code_concat(iloc_code_t* dest, iloc_code_t* src)` - concatena códigos
- `iloc_code_free(iloc_code_t* code)` - libera memória

---

## FASE 2: Geração de Nomes (Rótulos e Temporários)

### Passo 2.1: Implementar contador global para rótulos
- Criar variável estática `label_counter` inicializada em 0
- Função `get_next_label()` que retorna string no formato "L1", "L2", "L3", etc.
- Seguir convenção: rótulos começam com "L" maiúsculo seguido de número

### Passo 2.2: Implementar contador global para temporários
- Criar variável estática `temp_counter` inicializada em 0
- Função `get_next_temp()` que retorna string no formato "r1", "r2", "r3", etc.
- Seguir convenção: registradores começam com "r" minúsculo seguido de número
- **IMPORTANTE**: Não usar rfp, rsp, rbss, rpc (são reservados)

### Passo 2.3: Testar geração de nomes
- Criar programa de teste simples que gera alguns rótulos e temporários
- Verificar se os nomes seguem a convenção correta
- Garantir que os contadores incrementam corretamente

---

## FASE 3: Endereçamento de Variáveis

### Passo 3.1: Estender estrutura de símbolo para armazenar offset
- Modificar `symbol_t` em `table.h` para incluir campo `int offset`
- Este campo armazenará o deslocamento em relação a `rfp` (variáveis locais) ou `rbss` (variáveis globais)

### Passo 3.2: Calcular offsets na declaração de variáveis locais
- No parser, ao declarar variável local, calcular seu offset
- Offset deve ser negativo em relação a `rfp` (cresce para baixo na pilha)
- Primeira variável local: offset = -4 (assumindo int = 4 bytes)
- Próximas variáveis: offset = offset_anterior - 4
- Armazenar offset no símbolo durante a declaração

### Passo 3.3: Calcular offsets na declaração de variáveis globais
- No parser, ao declarar variável global, calcular seu offset
- Offset deve ser positivo em relação a `rbss`
- Primeira variável global: offset = 0
- Próximas variáveis: offset = offset_anterior + 4
- Armazenar offset no símbolo durante a declaração

### Passo 3.4: Criar função para obter endereço de variável
- Função `get_variable_address(symbol_t* symbol)` que retorna código ILOC para carregar endereço
- Para variável local: usar `loadAI rfp, offset => temp`
- Para variável global: usar `loadAI rbss, offset => temp`
- Retornar temporário contendo o endereço

---

## FASE 4: Geração de Código para Expressões Simples

### Passo 4.1: Implementar geração para literais inteiros
- Criar função `gen_literal_code(asd_tree_t* node)` 
- Para literal inteiro, gerar: `loadI valor => temp`
- Retornar código ILOC e temporário onde está o valor

### Passo 4.2: Implementar geração para identificadores (variáveis)
- Criar função `gen_identifier_code(asd_tree_t* node, stack_t* scopes)`
- Buscar símbolo na tabela de símbolos
- Obter offset do símbolo
- Gerar código para carregar valor da variável:
  - Local: `loadAI rfp, offset => temp`
  - Global: `loadAI rbss, offset => temp`
- Retornar código ILOC e temporário com o valor

### Passo 4.3: Implementar geração para operações aritméticas binárias (+, -, *, /)
- Criar função `gen_binary_arithmetic_code(char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes)`
- Recursivamente gerar código para operando esquerdo e direito
- Obter temporários resultantes de cada operando
- Gerar operação ILOC correspondente:
  - `+` → `add temp1, temp2 => temp3`
  - `-` → `sub temp1, temp2 => temp3`
  - `*` → `mult temp1, temp2 => temp3`
  - `/` → `div temp1, temp2 => temp3`
- Concatenar códigos dos operandos + operação
- Retornar código completo e temporário resultado

### Passo 4.4: Implementar otimização para constantes imediatas
- Quando um operando é constante, usar versão imediata da operação quando possível
- Exemplo: `addI temp1, 5 => temp2` ao invés de `loadI 5 => temp; add temp1, temp => temp2`
- Aplicar para: addI, subI, multI, divI

### Passo 4.5: Implementar geração para operadores unários (+, -, !)
- Criar função `gen_unary_code(char* op, asd_tree_t* operand, stack_t* scopes)`
- Para `+`: apenas retornar código do operando (sem operação)
- Para `-`: gerar código do operando, depois `multI temp, -1 => temp2`
- Para `!`: usar operação lógica `xorI temp, 1 => temp2` (assumindo 0=false, 1=true)

---

## FASE 5: Geração de Código para Expressões Lógicas

### Passo 5.1: Implementar comparações relacionais (<, <=, >, >=, ==, !=)
- Criar função `gen_relational_code(char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes)`
- Gerar código para operandos esquerdo e direito
- Gerar operação de comparação ILOC:
  - `<` → `cmp_LT temp1, temp2 => temp3`
  - `<=` → `cmp_LE temp1, temp2 => temp3`
  - `>` → `cmp_GT temp1, temp2 => temp3`
  - `>=` → `cmp_GE temp1, temp2 => temp3`
  - `==` → `cmp_EQ temp1, temp2 => temp3`
  - `!=` → `cmp_NE temp1, temp2 => temp3`
- Retornar código e temporário booleano (0 ou 1)

### Passo 5.2: Implementar operações lógicas AND e OR
- Criar função `gen_logical_code(char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes)`
- Gerar código para operandos esquerdo e direito
- Gerar operação ILOC:
  - `&&` → `and temp1, temp2 => temp3`
  - `||` → `or temp1, temp2 => temp3`
- Retornar código e temporário resultado

---

## FASE 6: Geração de Código para Comando de Atribuição

### Passo 6.1: Implementar geração básica de atribuição
- Criar função `gen_assignment_code(asd_tree_t* identifier, asd_tree_t* expression, stack_t* scopes)`
- Gerar código para a expressão (obter valor em temporário)
- Buscar símbolo do identificador
- Obter offset do símbolo
- Gerar operação de armazenamento:
  - Local: `storeAI temp_valor => rfp, offset`
  - Global: `storeAI temp_valor => rbss, offset`
- Concatenar código da expressão + store
- Retornar código completo

---

## FASE 7: Geração de Código para Fluxo de Controle

### Passo 7.1: Implementar geração para IF-THEN
- Criar função `gen_if_code(asd_tree_t* condition, asd_tree_t* then_block, stack_t* scopes)`
- Gerar código para condição (obter temporário booleano)
- Criar rótulos: `L_then` e `L_end`
- Gerar: `cbr temp_cond => L_then, L_end`
- Gerar código do bloco then com rótulo `L_then:`
- Gerar rótulo `L_end:`
- Concatenar tudo na ordem correta

### Passo 7.2: Implementar geração para IF-THEN-ELSE
- Estender função anterior para incluir bloco else
- Criar rótulos: `L_then`, `L_else`, `L_end`
- Gerar: `cbr temp_cond => L_then, L_else`
- Gerar código then com rótulo `L_then:`
- Gerar: `jumpI -> L_end`
- Gerar código else com rótulo `L_else:`
- Gerar rótulo `L_end:`

### Passo 7.3: Implementar geração para WHILE
- Criar função `gen_while_code(asd_tree_t* condition, asd_tree_t* body, stack_t* scopes)`
- Criar rótulos: `L_loop`, `L_body`, `L_end`
- Gerar rótulo `L_loop:`
- Gerar código da condição
- Gerar: `cbr temp_cond => L_body, L_end`
- Gerar código do corpo com rótulo `L_body:`
- Gerar: `jumpI -> L_loop`
- Gerar rótulo `L_end:`

---

## FASE 8: Integração com o Parser

### Passo 8.1: Adicionar campo ILOC na estrutura AST
- Modificar `asd_tree_t` em `asd.h` para incluir campo `iloc_code_t* iloc_code`
- Este campo armazenará o código ILOC gerado para cada nó
- Inicializar como NULL em `asd_new()`

### Passo 8.2: Criar função genérica de geração de código
- Criar função `iloc_code_t* generate_code(asd_tree_t* node, stack_t* scopes)`
- Esta função será chamada durante as reduções do parser
- Usar switch/case baseado no `label` do nó para chamar função específica
- Retornar código ILOC gerado

### Passo 8.3: Integrar geração no parser para expressões
- Nas regras de expressão do parser (`expr_add`, `expr_mul`, etc.), após criar o nó:
  - Chamar `generate_code()` para gerar código ILOC
  - Armazenar código no campo `iloc_code` do nó
- Fazer isso recursivamente para todos os nós de expressão

### Passo 8.4: Integrar geração no parser para comandos
- Nas regras de comandos (`comando_atribuicao`, `fluxo_condicional`, etc.):
  - Chamar `generate_code()` após criar o nó
  - Armazenar código ILOC no nó
- Para sequências de comandos, concatenar códigos ILOC de todos os comandos

### Passo 8.5: Integrar geração no parser para função principal
- Na regra `definicao_funcao`:
  - Gerar código para o corpo da função
  - Armazenar código ILOC completo no nó raiz da função
- Na raiz do programa (`programa`):
  - Obter código ILOC da função principal
  - Armazenar na raiz da AST

---

## FASE 9: Impressão do Código ILOC

### Passo 9.1: Implementar função de impressão de operando
- Criar função `iloc_print_operand(iloc_operand_t* op, FILE* out)`
- Imprimir de acordo com o tipo:
  - Registrador: imprimir string do registrador
  - Constante: imprimir número
  - Rótulo: imprimir string do rótulo

### Passo 9.2: Implementar função de impressão de operação
- Criar função `iloc_print_operation(iloc_operation_t* op, FILE* out)`
- Imprimir opcode
- Imprimir operandos fonte separados por vírgula
- Imprimir `=>` ou `->` dependendo do tipo de operação
- Imprimir operandos alvo separados por vírgula

### Passo 9.3: Implementar função de impressão de código completo
- Criar função `iloc_print_code(iloc_code_t* code, FILE* out)`
- Iterar sobre todas as operações
- Para cada operação, verificar se precisa de rótulo
- Imprimir no formato: `[op1; op2; op3]` ou `op1` (dependendo do número de operações)
- Imprimir uma operação por linha quando não agrupadas

### Passo 9.4: Modificar main.c para imprimir ILOC
- Substituir `asd_print_graphviz(arvore)` por impressão do código ILOC
- Obter código ILOC da raiz da AST
- Imprimir na saída padrão usando `iloc_print_code()`
- Manter compatibilidade com formato esperado pelo simulador

---

## FASE 10: Testes e Validação

### Passo 10.1: Testar expressões simples
- Criar teste com apenas uma expressão aritmética simples: `2 + 3`
- Verificar se gera código ILOC correto
- Executar no simulador `ilocsim.py` e verificar resultado

### Passo 10.2: Testar atribuições
- Criar teste com declaração e atribuição de variável
- Verificar se offsets estão corretos
- Verificar se store está usando endereço correto

### Passo 10.3: Testar fluxo de controle simples
- Criar teste com IF simples
- Verificar se rótulos estão sendo gerados corretamente
- Verificar se desvios condicionais estão corretos

### Passo 10.4: Testar programas completos
- Executar testes da pasta `tests/` que são válidos
- Comparar saída com resultado esperado
- Ajustar conforme necessário

### Passo 10.5: Validar com simulador
- Para cada teste, executar: `./etapa5 < teste.txt | python3 ilocsim.py`
- Verificar se estado final da memória está correto
- Corrigir bugs encontrados

---

## FASE 11: Refinamentos e Otimizações

### Passo 11.1: Otimizar uso de temporários
- Implementar reutilização de temporários quando possível
- Reduzir número de registradores usados

### Passo 11.2: Verificar tratamento de erros
- Garantir que código ILOC não é gerado se houver erros semânticos
- Validar que todas as variáveis foram declaradas antes do uso

### Passo 11.3: Documentar código
- Adicionar comentários explicativos nas funções principais
- Documentar convenções e decisões de design

---

## Checklist Final

- [ ] Estruturas de dados ILOC implementadas
- [ ] Geração de rótulos e temporários funcionando
- [ ] Endereçamento de variáveis (locais e globais) funcionando
- [ ] Expressões aritméticas gerando código correto
- [ ] Expressões lógicas gerando código correto
- [ ] Atribuições gerando código correto
- [ ] IF-THEN-ELSE gerando código correto
- [ ] WHILE gerando código correto
- [ ] Código ILOC sendo impresso na saída padrão
- [ ] Todos os testes passando no simulador
- [ ] Código limpo e documentado

---

## Notas Importantes

1. **Tradução Dirigida pela Sintaxe**: A geração de código deve acontecer durante as reduções do parser, não em uma passagem separada.

2. **Convenções ILOC**:
   - Rótulos: `L1`, `L2`, `L3`, ... (sempre começam com L maiúsculo)
   - Temporários: `r1`, `r2`, `r3`, ... (sempre começam com r minúsculo)
   - Registradores reservados: `rfp`, `rsp`, `rbss`, `rpc` (não usar como temporários)

3. **Endereçamento**:
   - Variáveis locais: offset negativo em relação a `rfp`
   - Variáveis globais: offset positivo em relação a `rbss`
   - Assumir que `int` ocupa 4 bytes

4. **Expressões Lógicas**: Usar tradução numérica (and/or) ao invés de controle de fluxo.

5. **Simplificações**: Não precisa implementar resto da divisão (`%`), apenas as outras operações.

6. **Testes**: Usar o simulador `ilocsim.py` fornecido para validar o código gerado.

---

## Ordem Sugerida de Implementação

Seguir esta ordem para garantir que cada passo possa ser testado independentemente:

1. Fase 1 completa (estruturas básicas)
2. Fase 2 completa (geração de nomes)
3. Fase 3 completa (endereçamento)
4. Passos 4.1 e 4.2 (literais e identificadores)
5. Passo 4.3 (operações aritméticas básicas)
6. Passo 6.1 (atribuições)
7. Testar com programa simples: `var x := inteiro; x := 5;`
8. Passos 5.1 e 5.2 (expressões lógicas)
9. Passos 7.1, 7.2, 7.3 (fluxo de controle)
10. Fase 8 completa (integração)
11. Fase 9 completa (impressão)
12. Fase 10 completa (testes)

Este roadmap garante que você possa testar incrementalmente e identificar problemas cedo no processo.

