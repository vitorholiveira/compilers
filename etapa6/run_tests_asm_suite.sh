#!/bin/bash

COMPILER="./etapa6"
TEST_DIR="tests-asm"

if [ ! -f "$COMPILER" ]; then
    echo "Compilador '$COMPILER' não encontrado. Rode 'make' antes."
    exit 1
fi

if [ ! -d "$TEST_DIR" ]; then
    echo "Diretório '$TEST_DIR' não encontrado."
    exit 1
fi

echo "Rodando suíte de testes ASM da Etapa 6..."
echo ""

# nome_do_teste expected_exit_code
tests=( 
  "t01_return_const.z 7"
  "t02_return_local.z 42"
  "t03_return_expr.z 14"
  # Retornar -2 no programa → código de saída do SO é 254
  "t04_return_expr_parens.z 254"
  "t05_while_counter.z 6"
  "t06_if_true.z 10"
  "t07_if_false.z 0"
  "t08_if_else.z 20"
  "t09_while_sum.z 10"
  "t10_global_basic.z 200"
  # Retorno 300 → código de saída 44 (300 mod 256)
  "t11_global_local_mix.z 44"
  "t12_cmp_lt_true.z 1"
  "t13_cmp_eq_false.z 0"
  "t14_logical_and.z 1"
  "t15_logical_or.z 1"
  "t16_logical_not.z 1"
  "t17_sequence.z 30"
  "t18_while_false_initial.z 0"
  "t19_if_else_chain.z 3"
)

passed=0
failed=0

for entry in "${tests[@]}"; do
    set -- $entry
    file="$1"
    expected="$2"

    src="$TEST_DIR/$file"
    base="${file%.z}"
    asm="${base}.s"
    bin="${base}.bin"

    if [ ! -f "$src" ]; then
        echo "[SKIP] $file (não encontrado)"
        continue
    fi

    echo "Teste: $file (esperado=$expected)"

    if ! ASAN_OPTIONS=detect_leaks=0 $COMPILER < "$src" > "$asm"; then
        echo "  [ERRO] compilação etapa6 falhou"
        failed=$((failed + 1))
        continue
    fi

    if ! gcc "$asm" -o "$bin"; then
        echo "  [ERRO] gcc falhou ao montar '$asm'"
        failed=$((failed + 1))
        continue
    fi

    "./$bin"
    rc=$?
    echo "  exit code obtido = $rc"

    if [ "$rc" -eq "$expected" ]; then
        echo "  [OK]"
        passed=$((passed + 1))
    else
        echo "  [FALHA] esperado=$expected, obtido=$rc"
        failed=$((failed + 1))
    fi

    echo ""
done

echo "Resumo: passaram=$passed, falharam=$failed"

exit 0


