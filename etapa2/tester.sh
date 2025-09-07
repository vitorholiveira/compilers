#!/bin/bash

# Verifica se o usuário passou um argumento
if [ -z "$1" ]; then
  echo "Uso: $0 arquivo.txt"
  exit 1
fi

ARQUIVO="$1"

# Verifica se o arquivo existe
if [ ! -f "$ARQUIVO" ]; then
  echo "Erro: arquivo '$ARQUIVO' não encontrado."
  exit 1
fi

# Executa o parser
cat "$ARQUIVO" | ./etapa2
RET=$?

# Mostra o código de saída
echo "Código de saída: $RET"
