#include "delta.c"
//#include "rice.h"
#include <iostream>
#include <cmath>

using namespace std;

const int NUM_SIZE = 32;
// auxiliary functions
int msb(uint v)
{
    int count=0;
    while(v>>=1 & 1)
    {
        count++;
    }
    return count;
}



typedef uint (*encodef)(uint* output, uint pos, uint value);
typedef uint (*decodef)(uint* input, uint pos, uint* value);

class Psums
{
	public:
	uint real;
	int pos;

	Psums()
	{
		this->real = 0;
		this->pos = 0;
	}	
	void setPos(int pos)
	{
		this->pos = pos;
	}
	void setReal(uint real)
	{
		this->real = real;
	}

};

class CompressedPsums
{
	private:
	Psums **s;
	uint *a;
	uint n;
	uint k;
	uint *d;
	uint size;
	uint *duplicate;
	encodef enc;
	decodef dec;

	public:
	CompressedPsums(uint *a,uint n, uint k,encodef enc,decodef dec)
	{
		uint NUM_BLOCKS = msb(n+2)+1;
		//cout << "NUM_BLOCKS = " << NUM_BLOCKS << endl;
		this->n = n;
		this->k = k;
		this->a = a;
		this->enc = enc;
		this->dec = dec;
		this->duplicate = new uint[n];
	//	cout << " n = " << n << endl;
	//	cout << " k = " << k << endl;
		for (int i = 0;i<n;i++)
		{
	//		cout << " a[" << i << "] = " << this->a[i] << endl;
		}
		
		this->s = new Psums*[NUM_BLOCKS];
		for (int i = 0 ; i < NUM_BLOCKS;i++)
		{
			this->s[i] = new Psums();
		}
		uint new_i = 0;
		int i = 0;
		while (new_i < n)
		{
			s[i]->real = a[new_i];
			i++;
	//		cout << "new_i = " << new_i << endl;
			new_i += pow(2,i);
			
		}
		cout << "Done" << endl;
	}

	uint * encode()
	{
		uint *b = new uint[this->n];
		for (int i = 0;i<n;i++)
		{
		//	cout << " a[" << i << "] = " << this->a[i] << endl;
			int delta = this->a[i] - this->a[i+1];
			b[i] = delta;
		//	cout << "delta = " << delta << endl;
		}

		uint *c = new uint[2];
		uint encode_length = 0;
		uint count = 0;
		for (int i = 0 ; i < n-1 ; i++)
		{
			if (b[i] != 0)
			{
				cout << " b[" << i << "] = " << b[i] << endl;
				count = 0;
				encode_length += this->enc(c,0,b[i]);
			}
			else
			{
				count++;
				duplicate[i+1] = count;
			}
		}
		this->size = encode_length;
		
		uint new_n = (encode_length/NUM_SIZE)+1;
		delete [] c;
		this->d = new uint[new_n];
		uint pos = 0;
		uint j = 0;
		for (int i = 1;i<n-1;i++)
		{
			int modulo = (pow(2,j+1))-1;
	//		cout << "modulo = " << modulo << endl;
	//		cout << "i = " << i << endl;

			if (modulo !=0)
			{
		//		cout << "Resultado = " << i%modulo << endl;
				if (i % modulo ==0)
				{
		//			cout << "j = " << j << endl;
		//			cout << " pos real = " << pos << endl;
					this->s[j]->pos = pos;
		//			cout << " bloque = " << this->s[j]->real << endl; 
					j++;
					
				}
				
			}
				
				if (b[i-1] != 0)
				pos += this->enc(this->d,pos,b[i-1]);
		}

		return d;
	}

	uint decode (uint pos)
	{

		cout << " ------- " << endl;
		cout << "pos recibido = " << pos << endl;
		int aux = 0;
		for (int i =0 ;i < pos ; i++)
		{
			if (duplicate[i] != 0 )
			{
				aux++;
			}
		}
		int new_pos;
		for (int i = 0 ; i < 15;i++)
		{
			cout << "pos = " << i << "duplicate:" << duplicate[i] << endl;
		}
		if (this->duplicate[pos] != 0)
		{
			pos = pos-this->duplicate[pos];
		}
		if (pos == 0)
		{
			new_pos = 0;
		}
		else
		{
				new_pos = log2(pos+2)-1;	
		}
		
		cout << "new_pos = " << new_pos << endl;

		uint real = this->s[new_pos]->real;
		cout << "real = " << real << endl;

		uint real_pos = this->s[new_pos]->pos;
		new_pos++;
		cout << "real_pos = " << real_pos << endl;
		int potencia = ((pow(2,new_pos)-2));
		cout << "potencia = " << potencia << endl;
		cout << "pos = " << pos << endl;

		if (pos == 0)
		{
			return real;
		}
		else
		{
			if (pos-potencia != 0)
			{
				if (duplicate[pos-1] != 1)
					new_pos = pos-potencia-duplicate[pos-1];
				else
					new_pos = pos-potencia;
			}
			else
					new_pos = pos-potencia;
			
		}
		cout << "new_new_pos = " << new_pos << endl;
		if (new_pos == 0)
		{
			return real;
		}
		int i = 0; 
		uint *r = new uint[1];
		while(i < new_pos)
		{
			real_pos += this->dec(this->d,real_pos,r);
			i++;
	//		cout << "------real = " << r[0] << endl;
			real -= r[0];
		}
		return real;
	}

	uint getSize()
	{
		return this->size;
	}

};

int main()
{
	int n =  15;
	int k = 1;
	uint A[15] = {100,80,80,45,45,34,23,12,11,8,8,8,8,1,1};

	CompressedPsums * ps = new CompressedPsums(A,n,k,encodeGamma,decodeGamma);
	ps->encode();
	cout << "size = " << ps->getSize();
	for (int i = 0 ;i < 15;i++)
	{
		cout << ps->decode(i) << endl;
	}
}


