#include <iostream>

using namespace std;

class myFunction{
public:
      int operator() (int x, int y) {
            return x + y;
      }

      int  operator *() {
            return *data;
      }



      void set(int x) {
            *data = x;
      }
private:
      int *data;
};

int main() {
      myFunction f;
      cout << "result = "<<f(1,2)<<endl;

      f.set(512);
      cout << "data = "<< *f <<endl;
}
