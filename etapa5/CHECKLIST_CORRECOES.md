# Checklist de Correções para Testes ILOC

## Status Atual
- **Testes Passando**: 9/100 (9%)
- **Testes Falhando**: 91/100 (91%)

## Problemas Identificados e Ações Necessárias

### 1. Problema: Código do `else` sendo concatenado antes do `jumpI` e `L1` (pass02)
**Sintoma**: O código do bloco `else` aparece antes do `jumpI -> L2` e `L1: nop`
**Testes Afetados**: pass02
**Ação Necessária**:
- [ ] Investigar por que `jumpI -> L2` e `L1: nop` não estão sendo adicionados ao código
- [ ] Verificar se há problema na ordem de execução em `gen_if_else_code`
- [ ] Verificar se `iloc_code_append` está funcionando corretamente após `iloc_code_concat`
- [ ] Verificar se há problema quando `then_block` é NULL (bloco vazio)

**Código Esperado**:
```
L0: nop
jumpI -> L2
L1: nop
loadI 2 => r1
storeAI r1 => rbss, 0
L2: nop
```

**Código Gerado**:
```
L0: nop
loadI 2 => r1
storeAI r1 => rbss, 0
L1: nop
```

---

### 2. Problema: Falta `L2: nop` no final do bloco da função (pass01)
**Sintoma**: Falta um rótulo `L2: nop` no final do código da função
**Testes Afetados**: pass01
**Ação Necessária**:
- [ ] Verificar se há necessidade de adicionar um rótulo final em funções que têm `if` sem `else`
- [ ] Verificar se `gen_if_code` precisa adicionar um rótulo extra no final
- [ ] Verificar se o problema está relacionado ao contador de labels

**Código Esperado**:
```
...
L1: nop
L2: nop
```

**Código Gerado**:
```
...
L1: nop
```

---

### 3. Problema: Código de estruturas aninhadas não está sendo gerado (pass06, pass07, pass08, pass09)
**Sintoma**: Apenas as primeiras 2 linhas do código são geradas, o resto está faltando
**Testes Afetados**: pass06, pass07, pass08, pass09
**Ação Necessária**:
- [ ] Verificar se `generate_block_code` está processando corretamente blocos aninhados
- [ ] Verificar se comandos dentro de blocos `if-else` aninhados estão sendo processados
- [ ] Verificar se há problema na recursão quando há `if-else` dentro de `if-else`
- [ ] Verificar se `generate_code` está sendo chamado corretamente para nós aninhados
- [ ] Verificar se há problema na ordem de processamento de filhos da AST

**Exemplo (pass06)**:
- Esperado: ~23 linhas de código ILOC
- Gerado: apenas 2 linhas (inicialização de variável)

---

### 4. Problema: Código de loops não está sendo gerado completamente (pass16, pass17, pass18)
**Sintoma**: Código do corpo do loop não está sendo gerado
**Testes Afetados**: pass16, pass17, pass18
**Ação Necessária**:
- [ ] Verificar se `gen_while_code` está gerando código corretamente
- [ ] Verificar se o corpo do loop está sendo processado por `generate_block_code`
- [ ] Verificar se há problema na geração de código para expressões dentro de loops
- [ ] Verificar se operações aritméticas (como `+ (-1)`) estão sendo geradas corretamente

**Exemplo (pass16)**:
- Esperado: código completo do loop `enquanto`
- Gerado: apenas inicialização de variável

---

### 5. Problema: Código sendo gerado quando não deveria (pass14, pass19)
**Sintoma**: Funções vazias estão gerando código ILOC quando não deveriam
**Testes Afetados**: pass14, pass19
**Ação Necessária**:
- [ ] Verificar se funções vazias (`[]`) estão sendo tratadas corretamente
- [ ] Verificar se `generate_block_code` retorna código vazio quando o bloco é NULL
- [ ] Verificar se há código sendo gerado durante o parsing que não deveria ser gerado
- [ ] Verificar se o problema está em `corpo_funcao` quando é vazio

**Exemplo (pass14)**:
- Esperado: nenhum código ILOC (ou apenas `loadAI rfp, 0 => r0` se há parâmetro)
- Gerado: código não esperado

**Exemplo (pass19)**:
- Esperado: nenhum código ILOC
- Gerado: `loadI 1 => r0` (não esperado)

---

## Priorização de Correções

### Alta Prioridade (Bloqueadores)
1. **pass02**: Problema crítico com `if-else` - afeta muitos outros testes
2. **pass06, pass07, pass08, pass09**: Código não está sendo gerado - problema estrutural grave
3. **pass16, pass17, pass18**: Loops não funcionam - problema estrutural grave

### Média Prioridade
4. **pass01**: Falta rótulo final - pode ser sintoma de problema maior
5. **pass14, pass19**: Código sendo gerado incorretamente - pode indicar problema no tratamento de blocos vazios

## Estratégia de Correção

1. **Primeiro**: Corrigir pass02 (problema com `if-else`)
   - Investigar por que `jumpI` e `L1` não aparecem no código gerado
   - Verificar ordem de concatenação em `gen_if_else_code`

2. **Segundo**: Corrigir pass06 (código aninhado não sendo gerado)
   - Verificar se `generate_block_code` está processando corretamente estruturas aninhadas
   - Verificar se há problema na recursão

3. **Terceiro**: Corrigir pass16 (loops não funcionam)
   - Verificar `gen_while_code`
   - Verificar se corpo do loop está sendo processado

4. **Quarto**: Corrigir pass01, pass14, pass19 (problemas menores)
   - Verificar tratamento de blocos vazios
   - Verificar rótulos finais

## Arquivos a Verificar

- `codegen.c`: 
  - `gen_if_else_code()` (linha ~870)
  - `gen_if_code()` (linha ~794)
  - `gen_while_code()` (linha ~971)
  - `generate_block_code()` (linha ~739)
  - `generate_code()` (linha ~580)

- `iloc.c`:
  - `iloc_code_append()` (linha ~220)
  - `iloc_code_concat()` (linha ~239)

- `parser.y`:
  - `corpo_funcao` (linha ~191)
  - `bloco_de_comandos` (linha ~245)
  - `fluxo_condicional` (linha ~386)

## Notas de Debug

- O problema em pass02 sugere que o código do `else` está sendo concatenado antes do `jumpI` e `L1` serem adicionados
- Isso pode indicar um problema na ordem de execução ou na forma como `iloc_code_concat` funciona
- Pode haver um problema quando o bloco `then` está vazio (NULL)

