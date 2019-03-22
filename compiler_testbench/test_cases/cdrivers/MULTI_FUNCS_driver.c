int f(int a);
int g_g();
int lol();
void lel(int* p);

int main() {
  int a = 1;
  int* p = &a;
  lel(p);
  lol();
  return a + lol() + g_g();
}