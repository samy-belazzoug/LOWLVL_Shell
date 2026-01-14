#!/bin/bash
# Comprehensive test suite for LOWLVL Shell

SHELL_PATH="/mnt/d/La Plateforme/2. Annee/LOWLVL_Shell/bashr"
PASS=0
FAIL=0

test_command() {
    local name="$1"
    local input="$2"
    local expected="$3"
    
    local output=$(echo -e "${input}\nexit" | ${SHELL_PATH} 2>&1 | tail -1)
    
    if [[ "$output" == *"$expected"* ]]; then
        echo "✓ PASS: $name"
        ((PASS++))
    else
        echo "✗ FAIL: $name"
        echo "  Expected: $expected"
        echo "  Got: $output"
        ((FAIL++))
    fi
}

echo "=== LOWLVL Shell Test Suite ==="
echo ""

# Test built-in commands
echo "Testing built-in commands..."
test_command "pwd" "pwd" "LOWLVL_Shell"
test_command "echo" "echo Hello" "Hello"

# Test pipes
echo ""
echo "Testing pipes..."
test_command "ls | grep md" "ls | grep .md" ".md"

# Test commands
echo ""
echo "Testing external commands..."
test_command "ls" "ls" "bashr"

echo ""
echo "=== Results ==="
echo "Passed: $PASS"
echo "Failed: $FAIL"

if [ $FAIL -eq 0 ]; then
    echo "✓ All tests passed!"
    exit 0
else
    echo "✗ Some tests failed"
    exit 1
fi
