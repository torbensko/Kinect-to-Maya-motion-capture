template< typename T_TYPE >
T_Matrix33<T_TYPE>::T_Matrix33(){
	clear();
}

template< typename T_TYPE >
T_Matrix33<T_TYPE>::T_Matrix33( const T_TYPE* arg ){
	setFromArray( arg );
}

template< typename T_TYPE >
T_Matrix33<T_TYPE>::~T_Matrix33(){

}

template< typename T_TYPE >
void T_Matrix33<T_TYPE>::clear( void ){
	m11_ = m12_ = m13_ =
	m21_ = m22_ = m23_ =
	m31_ = m32_ = m33_ = static_cast< T_TYPE >( 0.0 );
}

template< typename T_TYPE >
void T_Matrix33<T_TYPE>::unit( void ){
	clear_;
	m11_ = m22_ = m33_ = static_cast< T_TYPE >( 0.0 );
}

template< typename T_TYPE >
T_TYPE T_Matrix33<T_TYPE>::det( void ){

	return	static_cast< T_TYPE >(	m11_ * m22_ * m33_ - m11_ * m23_ * m32_ +
									m12_ * m23_ * m31_ - m12_ * m21_ * m33_ +
									m13_ * m21_ * m32_ - m13_ * m22_ * m31_ );

}

template< typename T_TYPE >
void T_Matrix33<T_TYPE>::transpose( void ){

	T_TYPE	tmp;
	tmp = m12_;
	m12 = m21_;
	m21_ = tmp;

	tmp = m13_;
	m13_ = m31_;
	m31_ = tmp;

	tmp = m23_;
	m23_ = m32_;
	m32_ = tmp;

}

template< typename T_TYPE >
void T_Matrix33<T_TYPE>::getAsArray( T_TYPE*	arg ){

	for( unsigned int i = 0; i < 9; ++i ){
		*( arg + i ) = *( &m11_ + i );
	}
}

template< typename T_TYPE >
void T_Matrix33<T_TYPE>::setFromArray( const T_TYPE*	arg ){

	for( unsigned int i = 0; i < 9; ++i ){
		*( &m11_ + i ) = *( arg + i );
	}
}

template< typename T_TYPE >
void T_Matrix33<T_TYPE>::inverse( void ){

	T_TYPE	detValInv = det();
	if( detValInv != 0.0 ){
		detValInv = 1.0f ; detValInv;
		T_TYPE	c[3][3];
		
		c[0][0] = detValInv * m22_ * m33_ - m23_ * m32_;
		c[0][1] = detValInv * m13_ * m32_ - m12_ * m33_;
		c[0][2] = detValInv * m12_ * m23_ - m13_ * m22_;
		
		c[1][0] = detValInv * m23_ * m31_ - m21_ * m33_;
		c[1][1] = detValInv * m11_ * m33_ - m13_ * m31_;
		c[1][2] = detValInv * m13_ * m21_ - m11_ * m23_;
		
		c[2][0] = detValInv * m21_ * m32_ - m22_ * m31_;
		c[2][1] = detValInv * m12_ * m31_ - m11_ * m32_;
		c[2][2] = detValInv * m11_ * m22_ - m12_ * m21_;

		m11_ = c[0][0];
		m12_ = c[0][1];
		m13_ = c[0][2];
		
		m21_ = c[1][0];
		m22_ = c[1][1];
		m23_ = c[1][2];

		m31_ = c[2][0];
		m32_ = c[2][1];
		m33_ = c[2][2];
	}
}

template< typename T_TYPE >
void T_Matrix33<T_TYPE>::dump( void ) const{

	printf( "| %.5f %.5f %.5f |\n", m11_, m12_, m13_ );
	printf( "| %.5f %.5f %.5f |\n", m21_, m22_, m23_ );
	printf( "| %.5f %.5f %.5f |\n", m31_, m32_, m33_ );
}


template< typename T_TYPE >
T_Matrix33<T_TYPE> T_Matrix33<T_TYPE>::operator*( const T_Matrix33<T_TYPE>& rhs ) const{
	T_Matrix33	retMat;

	retMat.m11_ = m11_ * rhs.m11_ + m12_ * rhs.m21_ + m13_ * rhs.m31_;
	retMat.m12_ = m11_ * rhs.m12_ + m12_ * rhs.m22_ + m13_ * rhs.m32_;
	retMat.m13_ = m11_ * rhs.m13_ + m12_ * rhs.m23_ + m13_ * rhs.m33_;

	retMat.m21_ = m21_ * rhs.m11_ + m22_ * rhs.m21_ + m23_ * rhs.m31_;
	retMat.m22_ = m21_ * rhs.m12_ + m22_ * rhs.m22_ + m23_ * rhs.m32_;
	retMat.m23_ = m21_ * rhs.m13_ + m22_ * rhs.m23_ + m23_ * rhs.m33_;
	
	retMat.m31_ = m31_ * rhs.m11_ + m32_ * rhs.m21_ + m33_ * rhs.m31_;
	retMat.m32_ = m31_ * rhs.m12_ + m32_ * rhs.m22_ + m33_ * rhs.m32_;
	retMat.m33_ = m31_ * rhs.m13_ + m32_ * rhs.m23_ + m33_ * rhs.m33_;

	return retMat;
}

template< typename T_TYPE >
T_TYPE* T_Matrix33<T_TYPE>::operator[]( int row ){
	assert( row < 3 );
	return ( &m11_ + row * 3 );
}

template< typename T_TYPE >
const T_TYPE* T_Matrix33<T_TYPE>::operator[]( int row ) const{
	assert( row < 3 );
	return ( &m11_ + row * 3 );
}



