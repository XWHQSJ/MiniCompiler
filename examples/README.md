# Example Programs

Each program can be run with:

```bash
echo "<input>" | ./build/minicc examples/<name>.pas --run
```

## factorial.pas

Computes n! using a recursive function.

| Input | Output |
|-------|--------|
| `0`   | `1`    |
| `1`   | `1`    |
| `5`   | `120`  |

```bash
echo "5" | ./build/minicc examples/factorial.pas --run
# 120
```

## sieve.pas

Prints all prime numbers up to N using trial division.

| Input | Output          |
|-------|-----------------|
| `10`  | `2 3 5 7`       |
| `20`  | `2 3 5 7 11 13 17 19` |
| `2`   | `2`             |

```bash
echo "10" | ./build/minicc examples/sieve.pas --run
# 2
# 3
# 5
# 7
```

## sort.pas

Sorts three integers using bubble sort with conditionals (no arrays).

| Input     | Output      |
|-----------|-------------|
| `3 1 2`   | `1 2 3`     |
| `5 5 5`   | `5 5 5`     |
| `9 1 4`   | `1 4 9`     |

```bash
echo "3 1 2" | ./build/minicc examples/sort.pas --run
# 1
# 2
# 3
```

## gcd.pas

Computes the greatest common divisor using the iterative Euclidean algorithm.

| Input     | Output |
|-----------|--------|
| `48 18`   | `6`    |
| `100 75`  | `25`   |
| `7 3`     | `1`    |

```bash
echo "48 18" | ./build/minicc examples/gcd.pas --run
# 6
```

## power.pas

Computes x^n using iterative multiplication.

| Input    | Output |
|----------|--------|
| `2 10`   | `1024` |
| `3 4`    | `81`   |
| `5 0`    | `1`    |

```bash
echo "2 10" | ./build/minicc examples/power.pas --run
# 1024
```

## hello.pas

Echoes a single integer (minimal test).

| Input | Output |
|-------|--------|
| `42`  | `42`   |
