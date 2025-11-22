# Análise das Falhas nos Testes do Professor

## Resumo
- **Total de testes que devem passar**: 100
- **Testes que passaram**: 4 (4%)
- **Testes que falharam**: 96 (96%)

## Problemas Identificados

### 1. Código ILOC Extra Sendo Gerado

O compilador está gerando código ILOC adicional que não deveria existir. Isso ocorre principalmente após atribuições e no final das funções.

#### Exemplo: `pass00`

**Código fonte:**
```
var b := inteiro,
f -> inteiro := [
  b := 2
];
```

**ILOC Esperado (do professor):**
```
loadI 2 => r0
storeAI r0 => rbss, 0
```

**ILOC Gerado (nosso código):**
```
loadI 2 => r1
storeAI r1 => rbss, 0
loadAI rbss, 0 => r2    ← CÓDIGO EXTRA
loadI 2 => r3            ← CÓDIGO EXTRA
```

**Problema**: Após a atribuição `b := 2`, o código está gerando instruções extras que parecem ser relacionadas ao retorno da função ou alguma expressão que não deveria estar lá.

---

### 2. Offsets Incorretos para Variáveis Locais

O cálculo de offsets para variáveis locais está usando valores negativos quando deveria usar valores positivos ou zero.

#### Exemplo: `pass01`

**Código fonte:**
```
var b := inteiro,
f -> inteiro := [
  var c := inteiro
  se (c) [
     b := 2
  ]
];
```

**ILOC Esperado:**
```
loadAI rfp, 0 => r0      ← offset 0
cbr r0 -> L0, L1
L0: nop
loadI 2 => r1
storeAI r1 => rbss, 0
L1: nop
L2: nop
```

**ILOC Gerado:**
```
loadAI rfp, -4 => r2      ← offset -4 (INCORRETO)
cbr r2 -> L1, L2
L1: loadI 2 => r3
storeAI r3 => rbss, 0
L2: nop
loadAI rfp, -4 => r4      ← CÓDIGO DUPLICADO
loadI 2 => r5
storeAI r5 => rbss, 0
```

**Problemas**:
1. Offset deveria ser `0` mas está sendo gerado como `-4`
2. Código está sendo gerado duas vezes (duplicação)
3. Rótulos diferentes (L0/L1 vs L1/L2)

---

### 3. Código Duplicado

Em vários testes, o mesmo código está sendo gerado múltiplas vezes, especialmente após blocos de controle.

#### Exemplo: `pass02`

**Código fonte:**
```
var b := inteiro,
f -> inteiro := [
  var c := inteiro
  se (c) [
  ]senao[
     b := 2
  ]
];
```

**ILOC Esperado:**
```
loadAI rfp, 0 => r0
cbr r0 -> L0, L1
L0: nop
jumpI -> L2
L1: nop
loadI 2 => r1
storeAI r1 => rbss, 0
L2: nop
```

**ILOC Gerado:**
```
loadAI rfp, -4 => r2
cbr r2 -> L1, L2
L1: loadI 2 => r3        ← CÓDIGO NO LUGAR ERRADO (deveria estar no else)
storeAI r3 => rbss, 0
L2: nop
loadAI rfp, -4 => r4      ← CÓDIGO DUPLICADO
loadI 2 => r5
storeAI r5 => rbss, 0
```

**Problemas**:
1. Código do `else` está sendo gerado no lugar errado
2. Código está sendo duplicado após o `if-else`
3. Falta o `jumpI -> L2` no bloco `then` vazio

---

### 4. Problemas com Loops (while)

#### Exemplo: `pass03`

**Código fonte:**
```
var b := inteiro,
f -> inteiro := [
  var c := inteiro
  enquanto (c) [
     b := 2
  ]
];
```

**ILOC Esperado:**
```
L0: nop                   ← Label do loop no início
loadAI rfp, 0 => r0
cbr r0 -> L1, L2
L1: nop
loadI 2 => r1
storeAI r1 => rbss, 0
jumpI -> L0              ← Volta para o início do loop
L2: nop
```

**ILOC Gerado:**
```
L1: nop                   ← Label diferente
loadAI rfp, -4 => r2      ← Offset incorreto
cbr r2 -> L2, L3
L2: loadI 2 => r3
storeAI r3 => rbss, 0
jumpI -> L1
L3: nop
loadAI rfp, -4 => r4      ← CÓDIGO DUPLICADO
loadI 2 => r5
storeAI r5 => rbss, 0
```

**Problemas**:
1. Label do loop diferente (L0 vs L1)
2. Offset incorreto (-4 vs 0)
3. Código duplicado após o loop

---

### 5. Problemas com Expressões de Retorno

O código parece estar gerando instruções para expressões de retorno mesmo quando não há retorno explícito.

#### Padrão Observado

Em quase todos os testes, após o código principal da função, há instruções extras como:
- `loadAI rbss, 0 => rX` (carregando variável global)
- `loadI <valor> => rX` (carregando constante)
- `storeAI rX => rfp, <offset>` (armazenando em variável local)

Isso sugere que o código está gerando instruções para uma expressão de retorno implícita ou para alguma expressão que não deveria estar sendo gerada.

---

## Comparação Detalhada: pass00

### Código Fonte
```
var b := inteiro,
f -> inteiro := [
  b := 2
];
```

### ILOC Esperado
```
loadI 2 => r0
storeAI r0 => rbss, 0
```

### ILOC Gerado
```
loadI 2 => r1
storeAI r1 => rbss, 0
loadAI rbss, 0 => r2
loadI 2 => r3
```

### Diferenças

1. **Registradores diferentes**: `r0` vs `r1` (isso é aceitável, mas o esperado usa r0)
2. **Código extra**: 
   - `loadAI rbss, 0 => r2` - carrega a variável `b` novamente
   - `loadI 2 => r3` - carrega a constante `2` novamente

### Análise

O código está gerando instruções extras que parecem ser:
- Uma tentativa de carregar o valor de `b` (que foi recém-atribuído)
- Uma tentativa de carregar a constante `2`

Isso sugere que há código sendo gerado para uma expressão de retorno ou para alguma expressão que não deveria estar sendo processada.

---

## Comparação Detalhada: pass01

### Código Fonte
```
var b := inteiro,
f -> inteiro := [
  var c := inteiro
  se (c) [
     b := 2
  ]
];
```

### ILOC Esperado
```
loadAI rfp, 0 => r0
cbr r0 -> L0, L1
L0: nop
loadI 2 => r1
storeAI r1 => rbss, 0
L1: nop
L2: nop
```

### ILOC Gerado
```
loadAI rfp, -4 => r2
cbr r2 -> L1, L2
L1: loadI 2 => r3
storeAI r3 => rbss, 0
L2: nop
loadAI rfp, -4 => r4
loadI 2 => r5
storeAI r5 => rbss, 0
```

### Diferenças

1. **Offset incorreto**: `rfp, 0` vs `rfp, -4`
2. **Rótulos diferentes**: L0/L1/L2 vs L1/L2
3. **Falta `nop` após `cbr`**: O esperado tem `L0: nop` logo após o `cbr`
4. **Código duplicado**: Todo o código está sendo gerado novamente após o `if`

### Análise

1. O offset `-4` sugere que o cálculo de offsets está considerando o frame pointer de forma diferente
2. A falta do `nop` após o `cbr` pode ser um problema de geração de rótulos
3. A duplicação do código sugere que há código sendo gerado duas vezes - uma vez para o bloco `if` e outra vez para algo mais (possivelmente uma expressão de retorno)

---

## Possíveis Causas

### 1. Geração de Código para Expressões de Retorno

O código pode estar gerando instruções para uma expressão de retorno implícita mesmo quando não há `retorna` explícito na função.

### 2. Cálculo de Offsets Incorreto

O cálculo de offsets para variáveis locais pode estar usando uma convenção diferente da esperada pelo professor.

### 3. Duplicação de Código

Pode haver um problema onde o código está sendo gerado múltiplas vezes - uma vez para o corpo da função e outra vez para alguma expressão adicional.

### 4. Geração de Rótulos

Os rótulos podem estar sendo gerados de forma diferente (começando em L1 ao invés de L0, ou usando uma numeração diferente).

### 5. Processamento de Blocos Vazios

Blocos vazios podem não estar sendo tratados corretamente, gerando código extra ou não gerando os `nop` necessários.

---

## Próximos Passos para Correção

1. **Investigar geração de código de retorno**: Verificar se há código sendo gerado para retornos implícitos
2. **Corrigir cálculo de offsets**: Ajustar o cálculo para usar offsets corretos (0 para primeira variável local)
3. **Eliminar duplicação**: Identificar por que o código está sendo gerado múltiplas vezes
4. **Ajustar geração de rótulos**: Garantir que os rótulos sejam gerados na ordem esperada
5. **Tratar blocos vazios**: Garantir que blocos vazios gerem apenas `nop` quando necessário

