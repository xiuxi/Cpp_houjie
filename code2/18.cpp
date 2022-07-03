using namespace std;

#include <iostream>

class CDocument {
public:
  void OnFileOpen() {
    cout << "dialog..." << endl;
    cout << "check file status..." << endl;
    cout << "open file ..." << endl;

    Serialize();

    cout << "close file" << endl;
    cout << "update all views..." << endl;
  }

  virtual void Serialize(){};
};

class CDoc : public CDocument {
public:
  virtual void Serialize() { cout << "CDoc::Serialize()" << endl; }
};

// void CDocument::OnFileOpen() {
//...
//   Serialize(); //实际形式:this->Serialize()
//...
// }
class CMyDoc : public CDocument {
  //   virtual Serialize(){}
};
int main() {
  CMyDoc myDoc;
  myDoc.OnFileOpen(); //实际形式:CDocument::OnFileOpen(&myDoc)
  return 0;
}