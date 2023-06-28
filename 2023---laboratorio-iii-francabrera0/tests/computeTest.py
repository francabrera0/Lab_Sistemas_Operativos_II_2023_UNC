
import filecmp

are_equal = filecmp.cmp('./tests/log/computeResults', './tests/computePattern')

if are_equal:
    print('Success')
else:
    print('Failed')