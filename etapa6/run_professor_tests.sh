#!/bin/bash

# Script para executar os testes do professor
# Testes em tests-iloc/pass devem passar (gerar ILOC correto)
# Testes em tests-iloc/failed devem falhar (gerar erro de compilação)

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Variáveis
# Para a etapa 5 (ILOC), use o binário original em ../etapa5
COMPILER="../etapa5/etapa5"
PASS_DIR="tests-iloc/pass"
FAILED_DIR="tests-iloc/failed"
VERBOSE=0
QUIET=0
DIFF_ONLY=0
SAVE_OUTPUT=0
OUTPUT_DIR="tests-iloc-generated"

# Contadores
TOTAL_PASS_TESTS=0
PASSED_PASS_TESTS=0
FAILED_PASS_TESTS=0

TOTAL_FAILED_TESTS=0
PASSED_FAILED_TESTS=0
FAILED_FAILED_TESTS=0

# Arrays para armazenar resultados
PASS_FAILED_LIST=()
FAILED_FAILED_LIST=()

# Função para mostrar ajuda
show_help() {
    echo "Uso: $0 [opções]"
    echo ""
    echo "Opções:"
    echo "  -v, --verbose    Mostra saída detalhada de cada teste"
    echo "  -q, --quiet      Mostra apenas resumo final"
    echo "  -d, --diff-only  Mostra apenas testes que falharam (com diff)"
    echo "  -s, --save       Salva ILOC gerado em $OUTPUT_DIR para análise"
    echo "  -h, --help       Mostra esta ajuda"
    echo ""
    echo "Exemplos:"
    echo "  $0                # Executa todos os testes com output padrão"
    echo "  $0 -v             # Executa com output detalhado"
    echo "  $0 -d             # Mostra apenas testes que falharam"
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
        -d|--diff-only)
            DIFF_ONLY=1
            shift
            ;;
        -s|--save)
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

# Verificar se os diretórios existem
if [ ! -d "$PASS_DIR" ]; then
    echo -e "${RED}Erro: Diretório '$PASS_DIR' não encontrado!${NC}"
    exit 1
fi

if [ ! -d "$FAILED_DIR" ]; then
    echo -e "${RED}Erro: Diretório '$FAILED_DIR' não encontrado!${NC}"
    exit 1
fi

# Criar diretório de outputs se necessário
if [ $SAVE_OUTPUT -eq 1 ]; then
    mkdir -p "$OUTPUT_DIR/pass" "$OUTPUT_DIR/failed"
fi

# Função para normalizar ILOC (remove espaços extras, linhas vazias no final)
normalize_iloc() {
    local file="$1"
    if [ ! -f "$file" ]; then
        echo ""
        return
    fi
    # Remove linhas vazias no final e espaços extras no final de cada linha
    sed 's/[[:space:]]*$//' "$file" | sed -e :a -e '/^\n*$/{$d;N;ba' -e '}'
}

# Função para comparar dois arquivos ILOC
compare_iloc() {
    local generated="$1"
    local expected="$2"
    
    # Normalizar ambos os arquivos
    local gen_normalized=$(normalize_iloc "$generated")
    local exp_normalized=$(normalize_iloc "$expected")
    
    # Comparar conteúdo normalizado
    if [ "$gen_normalized" = "$exp_normalized" ]; then
        return 0
    else
        return 1
    fi
}

# Função para executar um teste que deve passar
run_pass_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file")
    local expected_iloc="${test_file}.iloc"
    local temp_iloc="/tmp/test_${test_name}_$$.iloc"
    
    TOTAL_PASS_TESTS=$((TOTAL_PASS_TESTS + 1))
    
    # Verificar se arquivo .iloc esperado existe
    if [ ! -f "$expected_iloc" ]; then
        if [ $QUIET -eq 0 ] && [ $DIFF_ONLY -eq 0 ]; then
            printf "%-50s " "$test_name"
            echo -e "${RED}[ERRO] Arquivo .iloc esperado não encontrado${NC}"
        fi
        FAILED_PASS_TESTS=$((FAILED_PASS_TESTS + 1))
        PASS_FAILED_LIST+=("$test_name (arquivo .iloc não encontrado)")
        return 1
    fi
    
    # Compilar e gerar código ILOC
    local output_file=""
    if [ $SAVE_OUTPUT -eq 1 ]; then
        output_file="$OUTPUT_DIR/pass/${test_name}.iloc"
    fi
    
    if [ $SAVE_OUTPUT -eq 1 ]; then
        if ! $COMPILER < "$test_file" > "$output_file" 2>/dev/null; then
            if [ $QUIET -eq 0 ] && [ $DIFF_ONLY -eq 0 ]; then
                printf "%-50s " "$test_name"
                echo -e "${RED}[FALHOU] Erro na compilação${NC}"
            fi
            if [ $VERBOSE -eq 1 ]; then
                echo "  Erro ao compilar $test_file"
                $COMPILER < "$test_file" 2>&1 | head -5
            fi
            FAILED_PASS_TESTS=$((FAILED_PASS_TESTS + 1))
            PASS_FAILED_LIST+=("$test_name (erro de compilação)")
            rm -f "$output_file"
            return 1
        fi
        temp_iloc="$output_file"
    else
        if ! $COMPILER < "$test_file" > "$temp_iloc" 2>/dev/null; then
            if [ $QUIET -eq 0 ] && [ $DIFF_ONLY -eq 0 ]; then
                printf "%-50s " "$test_name"
                echo -e "${RED}[FALHOU] Erro na compilação${NC}"
            fi
            if [ $VERBOSE -eq 1 ]; then
                echo "  Erro ao compilar $test_file"
                $COMPILER < "$test_file" 2>&1 | head -5
            fi
            FAILED_PASS_TESTS=$((FAILED_PASS_TESTS + 1))
            PASS_FAILED_LIST+=("$test_name (erro de compilação)")
            rm -f "$temp_iloc"
            return 1
        fi
    fi
    
    # Comparar ILOC gerado com esperado
    if compare_iloc "$temp_iloc" "$expected_iloc"; then
        if [ $QUIET -eq 0 ] && [ $DIFF_ONLY -eq 0 ]; then
            printf "%-50s " "$test_name"
            echo -e "${GREEN}[OK]${NC}"
        fi
        PASSED_PASS_TESTS=$((PASSED_PASS_TESTS + 1))
        if [ $SAVE_OUTPUT -eq 0 ]; then
            rm -f "$temp_iloc"
        fi
        return 0
    else
        if [ $QUIET -eq 0 ]; then
            printf "%-50s " "$test_name"
            echo -e "${RED}[FALHOU] ILOC não corresponde${NC}"
        fi
        if [ $VERBOSE -eq 1 ] || [ $DIFF_ONLY -eq 1 ]; then
            echo ""
            echo -e "${CYAN}Diferença para $test_name:${NC}"
            echo -e "${YELLOW}--- Esperado${NC}"
            echo -e "${YELLOW}+++ Gerado${NC}"
            diff -u "$expected_iloc" "$temp_iloc" || true
            echo ""
        fi
        FAILED_PASS_TESTS=$((FAILED_PASS_TESTS + 1))
        PASS_FAILED_LIST+=("$test_name (ILOC não corresponde)")
        if [ $SAVE_OUTPUT -eq 0 ]; then
            rm -f "$temp_iloc"
        fi
        return 1
    fi
}

# Função para executar um teste que deve falhar
run_failed_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file")
    
    TOTAL_FAILED_TESTS=$((TOTAL_FAILED_TESTS + 1))
    
    # Tentar compilar - deve falhar
    if $COMPILER < "$test_file" > /dev/null 2>&1; then
        # Compilação bem-sucedida quando deveria falhar
        if [ $QUIET -eq 0 ] && [ $DIFF_ONLY -eq 0 ]; then
            printf "%-50s " "$test_name"
            echo -e "${RED}[FALHOU] Deveria ter gerado erro mas compilou com sucesso${NC}"
        fi
        if [ $VERBOSE -eq 1 ]; then
            echo "  Teste deveria falhar mas compilou: $test_file"
        fi
        FAILED_FAILED_TESTS=$((FAILED_FAILED_TESTS + 1))
        FAILED_FAILED_LIST+=("$test_name (deveria falhar)")
        return 1
    else
        # Compilação falhou como esperado
        if [ $QUIET -eq 0 ] && [ $DIFF_ONLY -eq 0 ]; then
            printf "%-50s " "$test_name"
            echo -e "${GREEN}[OK]${NC}"
        fi
        PASSED_FAILED_TESTS=$((PASSED_FAILED_TESTS + 1))
        return 0
    fi
}

# Cabeçalho
if [ $QUIET -eq 0 ]; then
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}  Testes do Professor - Etapa 5${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
    echo "Compilador: $COMPILER"
    echo "Diretório pass: $PASS_DIR"
    echo "Diretório failed: $FAILED_DIR"
    if [ $SAVE_OUTPUT -eq 1 ]; then
        echo "Salvando ILOCs gerados em: $OUTPUT_DIR"
    fi
    echo ""
fi

# Executar testes que devem passar
if [ $QUIET -eq 0 ]; then
    echo -e "${CYAN}Testes que devem passar:${NC}"
fi

pass_tests=$(find "$PASS_DIR" -maxdepth 1 -type f ! -name "*.iloc" | sort)

if [ -z "$pass_tests" ]; then
    echo -e "${YELLOW}Aviso: Nenhum teste encontrado em '$PASS_DIR'!${NC}"
else
    while IFS= read -r test_file; do
        run_pass_test "$test_file"
    done <<< "$pass_tests"
fi

echo ""

# Executar testes que devem falhar
if [ $QUIET -eq 0 ]; then
    echo -e "${CYAN}Testes que devem falhar:${NC}"
fi

failed_tests=$(find "$FAILED_DIR" -maxdepth 1 -type f | sort)

if [ -z "$failed_tests" ]; then
    echo -e "${YELLOW}Aviso: Nenhum teste encontrado em '$FAILED_DIR'!${NC}"
else
    while IFS= read -r test_file; do
        run_failed_test "$test_file"
    done <<< "$failed_tests"
fi

# Resumo final
echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Resumo dos Testes${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Resumo dos testes que devem passar
echo -e "${CYAN}Testes que devem passar:${NC}"
echo -e "  Total:     ${BLUE}$TOTAL_PASS_TESTS${NC}"
echo -e "  Passaram:  ${GREEN}$PASSED_PASS_TESTS${NC}"
echo -e "  Falharam:  ${RED}$FAILED_PASS_TESTS${NC}"

if [ $TOTAL_PASS_TESTS -gt 0 ]; then
    pass_rate=$(echo "scale=2; $PASSED_PASS_TESTS * 100 / $TOTAL_PASS_TESTS" | bc 2>/dev/null || echo "0")
    echo -e "  Taxa:      ${GREEN}${pass_rate}%${NC}"
fi

echo ""

# Resumo dos testes que devem falhar
echo -e "${CYAN}Testes que devem falhar:${NC}"
echo -e "  Total:     ${BLUE}$TOTAL_FAILED_TESTS${NC}"
echo -e "  Passaram:  ${GREEN}$PASSED_FAILED_TESTS${NC}"
echo -e "  Falharam:  ${RED}$FAILED_FAILED_TESTS${NC}"

if [ $TOTAL_FAILED_TESTS -gt 0 ]; then
    failed_rate=$(echo "scale=2; $PASSED_FAILED_TESTS * 100 / $TOTAL_FAILED_TESTS" | bc 2>/dev/null || echo "0")
    echo -e "  Taxa:      ${GREEN}${failed_rate}%${NC}"
fi

echo ""

# Resumo geral
TOTAL_TESTS=$((TOTAL_PASS_TESTS + TOTAL_FAILED_TESTS))
TOTAL_PASSED=$((PASSED_PASS_TESTS + PASSED_FAILED_TESTS))
TOTAL_FAILED=$((FAILED_PASS_TESTS + FAILED_FAILED_TESTS))

echo -e "${CYAN}Resumo Geral:${NC}"
echo -e "  Total:     ${BLUE}$TOTAL_TESTS${NC}"
echo -e "  Passaram:  ${GREEN}$TOTAL_PASSED${NC}"
echo -e "  Falharam:  ${RED}$TOTAL_FAILED${NC}"

if [ $TOTAL_TESTS -gt 0 ]; then
    overall_rate=$(echo "scale=2; $TOTAL_PASSED * 100 / $TOTAL_TESTS" | bc 2>/dev/null || echo "0")
    echo -e "  Taxa:      ${GREEN}${overall_rate}%${NC}"
fi

echo ""

# Mostrar lista de testes que falharam (que devem passar)
if [ ${#PASS_FAILED_LIST[@]} -gt 0 ]; then
    echo -e "${RED}Testes que deveriam passar mas falharam (${#PASS_FAILED_LIST[@]}):${NC}"
    for test in "${PASS_FAILED_LIST[@]}"; do
        echo "  - $test"
    done
    echo ""
fi

# Mostrar lista de testes que falharam (que devem falhar)
if [ ${#FAILED_FAILED_LIST[@]} -gt 0 ]; then
    echo -e "${RED}Testes que deveriam falhar mas passaram (${#FAILED_FAILED_LIST[@]}):${NC}"
    for test in "${FAILED_FAILED_LIST[@]}"; do
        echo "  - $test"
    done
    echo ""
fi

# Status final
if [ $TOTAL_FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ Todos os testes passaram!${NC}"
    exit 0
else
    echo -e "${RED}✗ Alguns testes falharam${NC}"
    exit 1
fi

