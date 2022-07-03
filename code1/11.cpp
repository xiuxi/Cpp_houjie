struct _List_node_base {
  _List_node_base *_M_next;
  _List_node_base *_M_prev;
};
template <typename _Tp>
struct _List_node : public _List_node_base // public 继承
{
  _Tp _M_data;
};

int main(int argc, char const *argv[])
{
    
    return 0;
}

