# Guia Rápido de Testes

## Executar Todos os Testes

### Método Recomendado: Script Completo

```bash
# Executar todos os testes
./run_tests.sh

# Com opções úteis:
./run_tests.sh -v          # Modo verbose (mostra detalhes de cada teste)
./run_tests.sh -q          # Modo quiet (apenas resumo final)
./run_tests.sh -o          # Salva código ILOC gerado em test_outputs/
./run_tests.sh --help      # Mostra todas as opções
```

### Método Alternativo: Script Simples

```bash
./run_tests_simple.sh
```

## Executar um Teste Específico

```bash
# Compilar e executar um teste específico
./etapa5 < tests/utest_expr_addition.txt | python3 ilocsim.py

# Ou salvar o código ILOC gerado
./etapa5 < tests/utest_expr_addition.txt > saida.iloc
python3 ilocsim.py < saida.iloc
```

## Ver Resultados

O script `run_tests.sh` mostra:
- ✅ Testes que passaram (em verde)
- ❌ Testes que falharam (em vermelho)
- ⚠️ Testes com erro de compilação (em vermelho)
- Resumo final com estatísticas

## Estrutura dos Testes

- **81 testes** organizados em categorias
- Todos os testes estão em `tests/utest_*.txt`
- Documentação completa em `tests/README_TESTES.md`

## Requisitos

- Compilador compilado: `./etapa5` (execute `make` primeiro)
- Simulador ILOC: `ilocsim.py` (deve estar no diretório raiz)
- Python 3 instalado

## Troubleshooting

**Erro: "Compilador não encontrado"**
```bash
make clean
make
```

**Erro: "Simulador não encontrado"**
- Verifique se `ilocsim.py` está no diretório raiz do projeto

**Teste falhando?**
- Execute com `-v` para ver detalhes: `./run_tests.sh -v`
- Ou execute o teste individualmente para debug

