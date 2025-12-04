## Etapa 6 – Roadmap de Geração de Código Assembly (x86_64)

### 1. Organização Geral do Projeto
- **1.1 Revisar saída da Etapa 5**
  - Verificar como o código ILOC é representado (estruturas, lista de instruções na raiz da AST, etc.).
  - Confirmar quais construções da linguagem já geram ILOC (declarações, expressões aritméticas, comparações, controle de fluxo, funções, etc.).
- **1.2 Definir ponto de entrada da geração de assembly**
  - Escolher/implementar uma função do compilador, por exemplo `gera_codigo_assembly()` (nome ilustrativo), que:
    - Receba a AST ou a estrutura que contém a lista de instruções ILOC.
    - Percorra a tabela de símbolos global.
    - Imprima na saída padrão todo o assembly (.data + .text).
- **1.3 Ajustar `main.c`**
  - Garantir que `main.c`:
    - Leia o programa da entrada padrão.
    - Construa a AST e gere o ILOC como na Etapa 5.
    - Chame a função de geração de assembly.
    - Termine sem mensagens extras (apenas o assembly na saída padrão).

### 2. Geração do Segmento de Dados (.data)
- **2.1 Analisar tabela de símbolos global**
  - Identificar todas as variáveis globais e suas informações: nome, tipo, tamanho, inicialização.
  - Verificar se funções também estão registradas na tabela global (para `.globl` e `.type`).
- **2.2 Definir convenções de tradução para globais**
  - Mapear tipos da linguagem para tamanhos em bytes e diretivas assembly adequadas (por exemplo, `int` → 4 bytes, usar `.long`).
  - Definir nomes de rótulos no assembly (normalmente o próprio identificador).
- **2.3 Implementar impressão do cabeçalho de dados**
  - Imprimir diretivas iniciais necessárias, por exemplo:
    - `.data`
    - `.align` apropriado (por exemplo, 4 para inteiros).
  - Para cada variável global:
    - Imprimir `.globl` se necessário.
    - Imprimir `.type` e `.size` (se fizer parte do padrão adotado, inspirado no `gcc`).
    - Definir o rótulo e a diretiva de inicialização (`.long <valor>` ou equivalente).
- **2.4 Verificar compatibilidade com `gcc`**
  - Gerar um exemplo simples com uma global e comparar com o assembly produzido pelo `gcc -S` para ajustar detalhes (.file, .text intermediário, etc.), se necessário.

### 3. Geração do Segmento de Código (.text) e Função `main`
- **3.1 Imprimir cabeçalho de texto**
  - Emissão de diretivas como:
    - `.text`
    - `.globl main`
    - `.type main, @function`
- **3.2 Implementar prólogo padrão de `main`**
  - Sequência típica:
    - `pushq %rbp`
    - `movq %rsp, %rbp`
    - Reserva de espaço na pilha para variáveis locais (ajuste de `%rsp`), se necessário.
- **3.3 Implementar epílogo padrão de `main`**
  - Restaurar pilha e base:
    - `leave` (ou `movq %rbp, %rsp` + `popq %rbp`)
    - `ret`
  - Garantir que o valor de retorno de `main` esteja em `%eax` antes do retorno.
- **3.4 Mapeamento das variáveis locais e parâmetros**
  - Definir como cada variável local será endereçada:
    - Por exemplo, `int` locais como deslocamentos negativos a partir de `%rbp` (`-4(%rbp)`, `-8(%rbp)`, etc.).
  - Se houver parâmetros de função (além de `main`), decidir:
    - Uso de convenção System V AMD64 (parâmetros em registradores `%rdi`, `%rsi`, etc.) ou um modelo simplificado coerente com o que o professor espera.

### 4. Mapeamento de Instruções ILOC → Assembly x86_64
- **4.1 Listar todas as instruções ILOC existentes na Etapa 5**
  - Operações aritméticas (add, sub, mul, div, etc.).
  - Operações de comparação (comp, cmp_LT, cmp_GT, etc.).
  - Operações lógicas (se existirem).
  - Carregamento/armazenamento (load, store, loadAI, storeAI, etc.).
  - Controle de fluxo (jump, cbr / branch condicional).
  - Instruções de chamada de função e retorno (se existirem).
- **4.2 Definir estratégia de alocação de registradores**
  - Decidir se:
    - Cada registrador ILOC mapeia para um registrador físico fixo (modelo simples com poucos registradores).
    - Ou se alguns valores vão diretamente para memória (modelo ainda mais simples).
  - Registrar claramente essa convenção em comentários no código.
- **4.3 Definir regras de tradução para cada instrução ILOC**
  - Para cada tipo de instrução, especificar:
    - Quais registradores x86_64 serão usados (%eax, %ebx, %ecx, %edx, etc.).
    - Quais instruções assembly correspondem (por exemplo, `addl`, `subl`, `imull`, `cmpl`, `jmp`, `je`, `jne`, `jl`, `jg`, etc.).
    - Quando será necessário gerar mais de uma instrução assembly para uma única instrução ILOC.
- **4.4 Implementar função de tradução de instruções ILOC**
  - Criar uma função, por exemplo `traduz_instrucao_iloc(ILOC *inst)`, que:
    - Receba uma instrução ILOC.
    - Gere a sequência de assembly correspondente com `printf`/`fprintf`.
  - Integrar essa função em um laço que percorre a lista de instruções ILOC da raiz da AST.
- **4.5 Gerenciar rótulos de controle de fluxo**
  - Definir convenção de nomes de rótulos para:
    - Destinos de saltos (labels de `jump`, `cbr`, etc.).
    - Inícios/fins de blocos de `while`, `if-else`, etc., se isso estiver refletido no ILOC.
  - Garantir unicidade dos rótulos (por exemplo, usando contador global).

### 5. Suporte a Comandos `return` e Finalização Correta
- **5.1 `return` na função principal (`main`)**
  - Garantir que a instrução (ou sequência) de ILOC que representa `return expr;`:
    - Avalie `expr` e coloque seu valor em um registrador ILOC específico.
    - Na tradução para assembly, mova o valor para `%eax`.
  - Saltar para o epílogo de `main` (ou gerar o epílogo logo após o código do `return`).
- **5.2 `return` em outras funções (se existirem)**
  - Definir prólogo/epílogo padrão para essas funções (pilha, base, salvamento de registradores, etc.).
  - Garantir que o valor de retorno esteja em `%eax` ao executar `ret`.

### 6. Integração, Testes e Ajustes Finais
- **6.1 Testar com exemplos minimalistas**
  - Criar pequenos programas em C, gerar assembly com `gcc -S` e comparar com:
    - Códigos equivalentes na linguagem do trabalho, compilados com seu compilador.
  - Usar diferenças apenas como guia; não é necessário replicar byte a byte, apenas seguir o padrão aceito.
- **6.2 Testes automáticos fornecidos pelo professor**
  - Executar:
    - `./etapa6 < exemplo.z > exemplo.s`
    - `gcc exemplo.s -o exemplo`
    - `./exemplo; echo $?`
  - Conferir se o valor de retorno corresponde ao esperado (por exemplo, o ex3 do enunciado deve retornar 6).
- **6.3 Tratar casos de erro comuns**
  - Verificar:
    - Se o assembly gerado não é aceito pelo `gcc` (erros de sintaxe, diretivas incorretas, rótulos ausentes).
    - Se há uso inconsistente de registradores (por exemplo, sobrescrever valores ainda necessários).
  - Corrigir o gerador de código com base nesses testes.
- **6.4 Limpeza e documentação**
  - Remover `printf` de debug e códigos mortos da geração de assembly.
  - Adicionar comentários explicando:
    - Convenção de pilha e registradores adotada.
    - Mapeamento ILOC → assembly.
    - Organização geral do gerador de código.

### 7. Checklist Resumido (para marcar progresso)
- **[ ]** Ajustar `main.c` para chamar o gerador de assembly.
- **[ ]** Implementar geração do segmento `.data` a partir da tabela de símbolos global.
- **[ ]** Implementar cabeçalho do `.text` e prólogo/epílogo de `main`.
- **[ ]** Definir e documentar convenção de registradores e endereçamento de variáveis.
- **[ ]** Mapear todas as instruções ILOC usadas na Etapa 5 para instruções x86_64.
- **[ ]** Implementar a função de tradução de uma instrução ILOC e o laço sobre a lista de instruções.
- **[ ]** Garantir suporte correto ao comando `return` na função `main`.
- **[ ]** (Se aplicável) Implementar suporte a outras funções além de `main`.
- **[ ]** Testar com exemplos simples, compilando com `gcc` e checando o código de retorno.
- **[ ]** Revisar, limpar o código e documentar as escolhas de implementação.


