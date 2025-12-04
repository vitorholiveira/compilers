#!/bin/bash

# Script simples para executar todos os testes
# Versão simplificada do run_tests.sh

# Para a etapa 5, use o binário original em ../etapa5
COMPILER="../etapa5/etapa5"
SIMULATOR="python3 ilocsim.py"
TESTS_DIR="tests"

# Cores
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

# Verificações básicas
if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}Erro: Compilador não encontrado! Execute 'make' primeiro.${NC}"
    exit 1
fi

if [ ! -f "ilocsim.py" ]; then
    echo -e "${RED}Erro: Simulador ilocsim.py não encontrado!${NC}"
    exit 1
fi

# Contadores
TOTAL=0
PASSED=0
FAILED=0

echo -e "${BLUE}Executando testes...${NC}"
echo ""

# Executar cada teste
for test_file in "$TESTS_DIR"/utest_*.txt; do
    if [ ! -f "$test_file" ]; then
        continue
    fi
    
    TOTAL=$((TOTAL + 1))
    test_name=$(basename "$test_file")
    
    # Compilar e executar com timeout de 1 segundo
    if timeout 1 bash -c "$COMPILER < \"$test_file\" 2>/dev/null | $SIMULATOR > /dev/null 2>&1" 2>/dev/null; then
        echo -e "${GREEN}✓${NC} $test_name"
        PASSED=$((PASSED + 1))
    else
        exit_code=$?
        if [ $exit_code -eq 124 ]; then
            echo -e "${RED}✗${NC} $test_name (TIMEOUT)"
        else
            echo -e "${RED}✗${NC} $test_name"
        fi
        FAILED=$((FAILED + 1))
    fi
done

echo ""
echo -e "${BLUE}Resumo:${NC}"
echo "Total: $TOTAL | Passaram: $PASSED | Falharam: $FAILED"

if [ $FAILED -eq 0 ]; then
    exit 0
else
    exit 1
fi

