class complex
{
	public:
		complex (double r = 0, double i = 0)
		: re (r), im (i)
		{ }
		complex& operator += (const complex&);
		double real () const { return re; }
		double imag () const { return im; }
	private:
		double re, im;
		friend complex& __doapl (complex*,
		const complex&);
};


class String
{
	public:
		String(const char* cstr = 0);
		String(const String& str);
		String& operator=(const String& str);
		~String();
		char* get_c_str() const { return m_data; }
	private:
		char* m_data;
};

int main() {

    return 0; 
}