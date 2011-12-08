#ifndef MATRIX_H_845451548628628652
#define MATRIX_H_845451548628628652

#include <cstdlib>
#include <memory>
#include <cmath>

static int realMod(int x, int mod) {
	int r = x % mod;
	if( r < 0 ) r += mod;
	return r;
}

template <typename T>
class Matrix {
public:
	class Cell {
	public:
		Cell()
		: matrix(NULL)
		{
		}
		
		Cell(int line, int column, Matrix<T>* matrix)
		: line(line), column(column), matrix(matrix)
		{
		}

		T& operator*() {
			return get();
		}
		
		T& get() {
			int realLine = realMod(line,matrix->lines);
			int realColumn = realMod(column,matrix->columns);
			return matrix->values[ realLine*matrix->columns + realColumn ];
		}
		
		T* operator->() {
			return &(get());
		}

		void operator=(const T& that) {
			(*(*this)) = that;
		}
		
		void operator=(const Cell& that) {
			this->line = that.line;
			this->column = that.column;
			this->matrix = that.matrix;
		}
		
		Cell neighbour(int lineOffset, int colOffset) {
			return Cell(line+lineOffset, column+colOffset, matrix);
		};
		
		bool overlapped() const {
			return line < 0 || column < 0 || line >= matrix->lines || column >= matrix->columns;
		};
		
		int x() const {
			return realMod(column,matrix->columns);
		}
		
		int y() const {
			return realMod(line,matrix->lines);
		};
		
		double distance( const Cell& that ) const {
			int deltaColumn = x()-that.x();
			int deltaLine = y()-that.y();
			double width = deltaColumn/double(matrix->width());
			double height = deltaLine/double(matrix->height());
			
			return sqrt(pow(width,2.0)+pow(height,2.0));
		};
	private:
		int line;
		int column;
		Matrix<T>* matrix;
	};
	class ConstCell {
	public:
		ConstCell()
		: matrix(NULL)
		{
		}
		
		ConstCell(int line, int column, const Matrix<T>* matrix)
		: line(line), column(column), matrix(matrix)
		{
		}

		const T& operator*() {
			return get();
		}
		
		const T& get() {
			int realLine = realMod(line,matrix->lines);
			int realColumn = realMod(column,matrix->columns);
			return matrix->values[ realLine*matrix->columns + realColumn ];
		}
		
		const T* operator->() {
			return &(get());
		}
		
		void operator=(const ConstCell& that) {
			this->line = that.line;
			this->column = that.column;
			this->matrix = that.matrix;
		}
		
		ConstCell neighbour(int lineOffset, int colOffset) {
			return ConstCell(line+lineOffset, column+colOffset, matrix);
		};
		
		bool overlapped() const {
			return line < 0 || column < 0 || line >= matrix->lines || column >= matrix->columns;
		};
		
		int x() const {
			return realMod(column,matrix->columns);
		}
		
		int y() const {
			return realMod(line,matrix->lines);
		};
		
		double distance( const Cell& that ) const {
			int deltaColumn = x()-that.x();
			int deltaLine = y()-that.y();
			double width = deltaColumn/double(matrix->width());
			double height = deltaLine/double(matrix->height());
			
			return sqrt(pow(width,2.0)+pow(height,2.0));
		};
	private:
		int line;
		int column;
		const Matrix<T>* matrix;
	};

	Matrix()
	: toDelete(true), values( NULL ), lines(0), columns(0)
	{
	}
	
	Matrix(T* values, unsigned int lines, unsigned int columns ):
		toDelete(false), values( values ), lines(lines), columns(columns)
	{
	}

	Matrix(unsigned int lines, unsigned int columns)
	: toDelete(true), values( new T[lines*columns] ), lines(lines), columns(columns)
	{
	}

	Matrix(const Matrix<T>& copy)
	: toDelete(true), values( new T[copy.lines*copy.columns] ), lines(copy.lines), columns(copy.columns)
	{
		for( int i=0;i<copy.lines*copy.columns;++i) {
			values[i] = copy.values[i];
		}
	}

	Matrix(Matrix<T>&& move)
	: toDelete(true), values( move.values ), lines(move.lines), columns(move.columns)
	{
		move.values = NULL;
	}
	
	void operator=(const Matrix<T>& copy) {
		values = new T[copy.lines*copy.columns];
		lines = copy.lines;
		columns = copy.columns;
		for( int i=0;i<copy.lines*copy.columns;++i) {
			values[i] = copy.values[i];
		}
	}
	
	void operator=(Matrix<T>&& move) {
		values = move.values;
		lines = move.lines;
		columns = move.columns;
		move.values = NULL;
	}
	
	virtual ~Matrix() {
		if( toDelete ) 
			delete[] values;
	}

	ConstCell get(int line, int column) const {
		return ConstCell(line,column,this);
	}
	
	ConstCell operator()(int line, int column) const {
		return ConstCell(line,column,this);
	}	
	Cell get(int line, int column) {
		return Cell(line,column,this);
	}
	
	Cell operator()(int line, int column) {
		return Cell(line,column,this);
	}
	
	unsigned int width() const {
		return columns;
	}
	unsigned int height() const {
		return lines;
	};
	
	T* asArray() const {
		return values;
	}
	
	void dotMax(const Matrix& that) {
		for( int line = 0; line < lines; ++line )
		for( int column = 0; column < columns; ++column ) {
			if( get(line,column).get() < that(line,column).get() )
				get(line,column) = that(line,column).get();
		}
	};	
	void dotMin(const Matrix& that) {
		for( int line = 0; line < lines; ++line )
		for( int column = 0; column < columns; ++column ) {
				if( get(line,column).get() > that(line,column).get() )
				get(line,column) = that(line,column).get();
		}
	};
private:
	bool toDelete;
	T* values;
	unsigned int lines;
	unsigned int columns;
};

#endif /* MATRIZ_H_ */
