#!/bin/bash

# Pequeno teste manual para a Etapa 6 (assembly).
# Gera um programa simples em nossa linguagem, compila com ./etapa6,
# monta com gcc e executa exibindo o código de saída.

set -e

COMPILER="./etapa6"
SRC_FILE="ex_ret0.z"
ASM_FILE="ex_ret0.s"
BIN_FILE="ex_ret0"

if [ ! -f "$COMPILER" ]; then
    echo "Compilador '$COMPILER' não encontrado. Rode 'make' antes."
    exit 1
fi

cat > "$SRC_FILE" << 'EOF'
main() -> int {
    return 0;
}
EOF

echo "Gerando assembly com $COMPILER..."
$COMPILER < "$SRC_FILE" > "$ASM_FILE"

echo "Montando com gcc..."
gcc "$ASM_FILE" -o "$BIN_FILE"

echo "Executando binário..."
./"$BIN_FILE"
echo "Código de saída (esperado 0): $?"


