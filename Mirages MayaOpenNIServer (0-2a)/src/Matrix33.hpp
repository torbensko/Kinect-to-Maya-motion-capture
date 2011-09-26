#if !defined( MIRAGE_CG_MATRIX33_H_INLUCDED__ )
#define MIRAGE_CG_MATRIX33_H_INLUCDED__

//--------------------------------------------------------
//! T_Matrix
//! 3x3 Matrix template class
//!
//--------------------------------------------------------
template< typename T_TYPE >
class T_Matrix33{

 public:
	//typedefs
	typedef		T_TYPE	TYPE_;

 public:
	//Members
	struct{
		T_TYPE	m11_, m12_, m13_;
		T_TYPE	m21_, m22_, m23_;
		T_TYPE	m31_, m32_, m33_;
	};

 public:
	//Creators
	inline T_Matrix33();
	inline T_Matrix33( const T_TYPE* );
	virtual inline ~T_Matrix33();

 public:
	//Manipulator
	inline void				clear			(	void );
	inline void				unit			(	void );
	inline T_TYPE			det				(	void );
	inline void				transpose		(	void );
	inline void				inverse			(	void );
	inline void				getAsArray		(	T_TYPE* );
	inline void				setFromArray	(	const T_TYPE* );

	inline void				dump			(	void ) const;

 public:
	//Operators
	T_Matrix33< T_TYPE >		operator *(		const T_Matrix33< T_TYPE>& ) const;
	inline T_TYPE*				operator [](	int );
	inline const T_TYPE*		operator [](	int ) const;

};

typedef T_Matrix33< float >	C_Matrix33;

#include "Matrix33.inl"

#endif