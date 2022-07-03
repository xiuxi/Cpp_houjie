template <class Key> struct hash {};

template <> struct hash<char> {
  size_t operator()(char x) const { return x; }
};

template <> struct hash<int> {
  size_t operator()(int x) const { return x; }
};

template <> struct hash<long> {
  size_t operator()(long x) const { return x; }
};

int main() {

    return 0;
}