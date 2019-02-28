
def collatz_recursive(n):
  if (n == 1):
    return n
  if ((n % 2) == 0):
    return (n + collatz_recursive((n / 2)))
  else:
    return (n + collatz_recursive(((n * 3) + 1)))


def collatz_iterative(n):
  res = 1
  while (n != 1):
    res = (res + n)
    if ((n % 2) == 0):
      n = (n / 2)
    else:
      n = ((n * 3) + 1)
  return res


def main():
  tmp = (20 % 3)
  n = ((5 * 3) + (20 - tmp))
  return (collatz_recursive(n) == collatz_iterative(n))


if __name__ == '__main__':
  import sys
  ret = main()
  sys.exit(ret)
