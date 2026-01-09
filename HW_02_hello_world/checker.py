#!/usr/bin/env python3

import os
import sys

MODULE_NAME = "hello_module"
PARAMS_PATH = f"/sys/module/{MODULE_NAME}/parameters"
EXPECTED = "Hello, World!"

ASCII_MAP = [(i, ord(c)) for i, c in enumerate(EXPECTED)]

def write_param(param, value):
    try:
        with open(f"{PARAMS_PATH}/{param}", "w") as f:
            f.write(str(value))
        return True
    except (PermissionError, OSError) as e:
        print(f"ERROR: cannot write {param}={value}: {e}")
        return False

def read_param(param):
    try:
        with open(f"{PARAMS_PATH}/{param}", "r") as f:
            return f.read().strip()
    except Exception as e:
        print(f"ERROR: cannot read {param}: {e}")
        return None

def test_readonly():
    try:
        with open(f"{PARAMS_PATH}/my_str", "w") as f:
            f.write("test")
        print("FAIL: my_str is writable!")
        return False
    except (PermissionError, OSError):
        print("OK: my_str is read-only")
        return True

def main():
    if not os.path.exists(PARAMS_PATH):
        print(f"ERROR: module '{MODULE_NAME}' not loaded")
        sys.exit(1)

    print("Testing read-only protection...")
    if not test_readonly():
        sys.exit(1)

    print("Writing 'Hello, World!' character by character...")
    for idx, ch in ASCII_MAP:
        if not write_param("int_idx", idx):
            sys.exit(1)
        if not write_param("char_val", ch):
            sys.exit(1)

    result = read_param("my_str")
    if result == EXPECTED:
        print(f"SUCCESS: got '{result}'")
    else:
        print(f"FAIL: expected '{EXPECTED}', got '{result}'")
        sys.exit(1)

if __name__ == "__main__":
    main()