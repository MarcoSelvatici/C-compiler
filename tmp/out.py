
def f(a):
  if (a > 0):
    while (a > 0):
      a = (a - 1)
  else:
    return a
  return (a + 1)


if __name__ == '__main__':
  import sys
  ret = main()
  sys.exit(ret)
