
def f(a):
  if (a < 5):
    return 1
  return a


def g(a):
  if (a > 0):
    if (a == 1):
      return 2
    else:
      return 3


if __name__ == '__main__':
  import sys
  ret = main()
  sys.exit(ret)
