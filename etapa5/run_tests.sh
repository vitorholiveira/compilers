#!/bin/bash

# Script para executar todos os testes da Etapa 5
# Uso: ./run_tests.sh [opções]
# Opções:
#   -v, --verbose    Mostra saída detalhada de cada teste
#   -q, --quiet      Mostra apenas resumo final
#   -o, --output     Salva saída ILOC de cada teste em arquivo
#   -h, --help       Mostra esta ajuda

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Variáveis
VERBOSE=0
QUIET=0
SAVE_OUTPUT=0
TESTS_DIR="tests"
COMPILER="./etapa5"
SIMULATOR="python3 ilocsim.py"
OUTPUT_DIR="test_outputs"

# Contadores
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
ERROR_TESTS=0

# Arrays para armazenar resultados
PASSED_LIST=()
FAILED_LIST=()
ERROR_LIST=()

# Função para mostrar ajuda
show_help() {
    echo "Uso: $0 [opções]"
    echo ""
    echo "Opções:"
    echo "  -v, --verbose    Mostra saída detalhada de cada teste"
    echo "  -q, --quiet      Mostra apenas resumo final"
    echo "  -o, --output     Salva saída ILOC de cada teste em arquivo"
    echo "  -h, --help       Mostra esta ajuda"
    echo ""
    echo "Exemplos:"
    echo "  $0                # Executa todos os testes com output padrão"
    echo "  $0 -v             # Executa com output detalhado"
    echo "  $0 -o             # Salva saída ILOC de cada teste"
}

# Parse argumentos
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=1
            shift
            ;;
        -q|--quiet)
            QUIET=1
            shift
            ;;
        -o|--output)
            SAVE_OUTPUT=1
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo "Opção desconhecida: $1"
            show_help
            exit 1
            ;;
    esac
done

# Verificar se o compilador existe
if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}Erro: Compilador '$COMPILER' não encontrado!${NC}"
    echo "Execute 'make' primeiro para compilar o projeto."
    exit 1
fi

# Verificar se o simulador existe
if [ ! -f "ilocsim.py" ]; then
    echo -e "${RED}Erro: Simulador 'ilocsim.py' não encontrado!${NC}"
    exit 1
fi

# Verificar se a pasta de testes existe
if [ ! -d "$TESTS_DIR" ]; then
    echo -e "${RED}Erro: Pasta '$TESTS_DIR' não encontrada!${NC}"
    exit 1
fi

# Criar diretório de outputs se necessário
if [ $SAVE_OUTPUT -eq 1 ]; then
    mkdir -p "$OUTPUT_DIR"
fi

# Função para executar um teste
run_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file")
    local output_file=""
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if [ $SAVE_OUTPUT -eq 1 ]; then
        output_file="$OUTPUT_DIR/${test_name%.txt}.iloc"
    fi
    
    # Mostrar progresso se não estiver em modo quiet
    if [ $QUIET -eq 0 ]; then
        printf "%-60s " "$test_name"
    fi
    
    # Compilar e gerar código ILOC
    if [ $SAVE_OUTPUT -eq 1 ]; then
        if ! $COMPILER < "$test_file" > "$output_file" 2>/dev/null; then
            ERROR_TESTS=$((ERROR_TESTS + 1))
            ERROR_LIST+=("$test_name")
            if [ $QUIET -eq 0 ]; then
                echo -e "${RED}[ERRO]${NC} Falha na compilação"
            fi
            if [ $VERBOSE -eq 1 ]; then
                echo "  Erro ao compilar $test_file"
            fi
            return 1
        fi
    else
        if ! $COMPILER < "$test_file" > /tmp/test_iloc_$$.iloc 2>/dev/null; then
            ERROR_TESTS=$((ERROR_TESTS + 1))
            ERROR_LIST+=("$test_name")
            if [ $QUIET -eq 0 ]; then
                echo -e "${RED}[ERRO]${NC} Falha na compilação"
            fi
            if [ $VERBOSE -eq 1 ]; then
                echo "  Erro ao compilar $test_file"
            fi
            return 1
        fi
    fi
    
    # Executar simulador
    local simulator_output
    if [ $SAVE_OUTPUT -eq 1 ]; then
        simulator_output=$($SIMULATOR < "$output_file" 2>&1)
    else
        simulator_output=$($SIMULATOR < /tmp/test_iloc_$$.iloc 2>&1)
    fi
    
    local simulator_exit_code=$?
    
    # Verificar resultado
    if [ $simulator_exit_code -eq 0 ]; then
        PASSED_TESTS=$((PASSED_TESTS + 1))
        PASSED_LIST+=("$test_name")
        if [ $QUIET -eq 0 ]; then
            echo -e "${GREEN}[OK]${NC}"
        fi
        if [ $VERBOSE -eq 1 ]; then
            echo "  ✓ Teste passou"
            if [ $VERBOSE -eq 1 ] && [ -n "$simulator_output" ]; then
                echo "  Saída do simulador:"
                echo "$simulator_output" | sed 's/^/    /'
            fi
        fi
        return 0
    else
        FAILED_TESTS=$((FAILED_TESTS + 1))
        FAILED_LIST+=("$test_name")
        if [ $QUIET -eq 0 ]; then
            echo -e "${RED}[FALHOU]${NC}"
        fi
        if [ $VERBOSE -eq 1 ]; then
            echo "  ✗ Teste falhou"
            echo "  Saída do simulador:"
            echo "$simulator_output" | sed 's/^/    /'
        fi
        return 1
    fi
}

# Limpar arquivo temporário ao sair
cleanup() {
    rm -f /tmp/test_iloc_$$.iloc
}
trap cleanup EXIT

# Cabeçalho
if [ $QUIET -eq 0 ]; then
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}  Executando Testes - Etapa 5${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
    echo "Compilador: $COMPILER"
    echo "Simulador: $SIMULATOR"
    echo "Diretório de testes: $TESTS_DIR"
    if [ $SAVE_OUTPUT -eq 1 ]; then
        echo "Salvando outputs em: $OUTPUT_DIR"
    fi
    echo ""
fi

# Encontrar e executar todos os testes
test_files=$(find "$TESTS_DIR" -name "utest_*.txt" | sort)

if [ -z "$test_files" ]; then
    echo -e "${RED}Erro: Nenhum teste encontrado em '$TESTS_DIR'!${NC}"
    exit 1
fi

# Executar cada teste
while IFS= read -r test_file; do
    run_test "$test_file"
done <<< "$test_files"

# Resumo final
echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Resumo dos Testes${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo -e "Total de testes:     ${BLUE}$TOTAL_TESTS${NC}"
echo -e "Testes passaram:     ${GREEN}$PASSED_TESTS${NC}"
echo -e "Testes falharam:     ${RED}$FAILED_TESTS${NC}"
echo -e "Testes com erro:     ${RED}$ERROR_TESTS${NC}"
echo ""

# Calcular porcentagem
if [ $TOTAL_TESTS -gt 0 ]; then
    success_rate=$(echo "scale=2; $PASSED_TESTS * 100 / $TOTAL_TESTS" | bc)
    echo -e "Taxa de sucesso:     ${GREEN}${success_rate}%${NC}"
    echo ""
fi

# Mostrar lista de testes que falharam
if [ ${#FAILED_LIST[@]} -gt 0 ]; then
    echo -e "${RED}Testes que falharam (${#FAILED_LIST[@]}):${NC}"
    for test in "${FAILED_LIST[@]}"; do
        echo "  - $test"
    done
    echo ""
fi

# Mostrar lista de testes com erro
if [ ${#ERROR_LIST[@]} -gt 0 ]; then
    echo -e "${RED}Testes com erro de compilação (${#ERROR_LIST[@]}):${NC}"
    for test in "${ERROR_LIST[@]}"; do
        echo "  - $test"
    done
    echo ""
fi

# Status final
if [ $FAILED_TESTS -eq 0 ] && [ $ERROR_TESTS -eq 0 ]; then
    echo -e "${GREEN}✓ Todos os testes passaram!${NC}"
    exit 0
else
    echo -e "${RED}✗ Alguns testes falharam ou tiveram erros${NC}"
    exit 1
fi

