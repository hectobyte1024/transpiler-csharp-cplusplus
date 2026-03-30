# Interactive Regex Validator User Guide

## Usage

```bash
./regex_validator <test_file.txt>
```

**Example:**
```bash
./regex_validator teacher_patterns.txt
```

---

## Input File Format

**Pattern Syntax:**
```
PATTERN: your_regex_here
DESC: Optional description
TESTS:
  test_string -> true
  test_string -> false
---
```

**Key Notes:**
- Each pattern block must end with `---` on its own line
- Use `->` (dash greater-than) as the separator, NOT Unicode arrow
- `true` means the regex SHOULD match the input
- `false` means the regex SHOULD NOT match the input
- Empty lines and lines starting with `#` are ignored
- Special characters in test strings:
  - `\n` for newline
  - `\t` for tab
  - `\r` for carriage return
  - Other backslash escapes pass through

---

## Example Test File

```
# Test 1: Literal string matching
PATTERN: hello
DESC: Match exactly "hello"
TESTS:
  hello -> true
  Hello -> false
  hell -> false
---

# Test 2: Digits
PATTERN: \d+
DESC: One or more digits
TESTS:
  123 -> true
  42 -> true
  abc -> false
  12abc -> false
---

# Test 3: Character class
PATTERN: [a-z]+
DESC: Lowercase letters
TESTS:
  abc -> true
  hello -> true
  ABC -> false
  abc123 -> false
---
```

---

## Output Explanation

### Per-Pattern Report
Shows:
- Pattern being tested
- Description
- Each test case with ✓ PASS or ✗ FAIL
- Pass rate percentage
- Failed test details

### Overall Statistics
Shows:
- Total patterns tested
- Patterns with 100% pass rate
- Total test cases
- Passed vs failed
- Overall pass rate percentage

---

## Common Patterns for Teachers

### Basic Matching
```
PATTERN: hello
DESC: Literal string
TESTS:
  hello -> true
  Hello -> false
  helloworld -> false
---
```

### Character Classes
```
PATTERN: [a-z]
DESC: Single lowercase letter
TESTS:
  a -> true
  z -> true
  A -> false
  1 -> false
---
```

### Quantifiers
```
PATTERN: a+
DESC: One or more 'a'
TESTS:
  a -> true
  aaa -> true
  b -> false
  -> false
---
```

### Alternation
```
PATTERN: cat|dog
DESC: Match cat or dog
TESTS:
  cat -> true
  dog -> true
  bird -> false
---
```

### Email-like
```
PATTERN: [a-z]+@[a-z]+
DESC: Simple email
TESTS:
  user@domain -> true
  test@example -> true
  @domain -> false
---
```

### Decimal Numbers
```
PATTERN: \d+\.\d+
DESC: Decimal numbers
TESTS:
  3.14 -> true
  0.5 -> true
  3. -> false
  .14 -> false
---
```

---

## Escape Sequences in Patterns

- `\d` = digit (0-9)
- `\w` = word character (a-z, A-Z, 0-9, _)
- `\s` = whitespace (space, tab, newline)
- `\n` = newline
- `\t` = tab
- `\.` = literal dot
- `\|` = literal pipe
- `\+` = literal plus
- `\*` = literal asterisk
- `\?` = literal question mark
- `\\` = literal backslash

---

## Tips

1. **Start Simple** - Test basic patterns first (literals, then character classes, then quantifiers)
2. **Test Edge Cases** - Empty strings, single characters, very long strings
3. **Use Descriptions** - Helps understand what each pattern should do
4. **Group Related Tests** - Keep related patterns together with clear separation

---

## Example Teacher's Test File

```
# Week 1: Basic Patterns
PATTERN: [a-zA-Z]+
DESC: Alphabetic characters only
TESTS:
  hello -> true
  world -> true
  Hello -> true
  hello123 -> false
  123 -> false
---

# Week 1: Numbers and Letters
PATTERN: [0-9a-zA-Z]+
DESC: Alphanumeric characters
TESTS:
  abc123 -> true
  test -> true
  123 -> true
  abc_123 -> false
---

# Week 2: Email validation
PATTERN: [a-z]+@[a-z]+\.com
DESC: Simple email with .com domain
TESTS:
  user@example.com -> true
  test@domain.com -> true
  User@Example.com -> false
  user@example -> false
---

# Week 2: Phone numbers
PATTERN: \d\d\d-\d\d\d-\d\d\d\d
DESC: Phone number XXX-XXX-XXXX
TESTS:
  123-456-7890 -> true
  555-867-5309 -> true
  1234567890 -> false
  123-45-6789 -> false
---
```
