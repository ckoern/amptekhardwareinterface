%module AmptekHardwareInterface

%{
    #define SWIG_FILE_WITH_INIT
    #include "AmptekHardwareInterface.h"
%}
%feature("autodoc");
%include "numpy.i"
%include "std_string.i"
%init %{
    import_array();
%}
%naturalvar AmptekSpectrum::bins;

%numpy_typemaps(unsigned int      , NPY_UINT     , int)

%include "stdint.i"
%include "std_vector.i"
%include "std_pair.i"

%include exception.i       
namespace std {
   %template(IntVector) vector<int>;
   %template(StringVector) vector<std::string>;
   %template(DoubleVector) vector<double>;
   %template(UIntVector) vector<unsigned int>;
   %template(AmptekDataPair) pair<AmptekSpectrum, AmptekStatus>;
}
%except {
	try {
		$function
	} catch(RangeError) {
		SWIG_exception(SWIG_ValueError, "Range Error");
	} catch(DivisionByZero) {
		SWIG_exception(SWIG_DivisionByZero, "Division by zero");
	} catch(OutOfMemory) {
		SWIG_exception(SWIG_MemoryError, "Out of memory");
	} catch(AmptekException) {
		SWIG_exception(SWIG_RuntimeError, "Amptek Error");
	} catch(...) {
		SWIG_exception(SWIG_RuntimeError,"Unknown exception");
	}
}

%include "include/AmptekHardwareInterface.h"
%include "include/AmptekStatus.h"
%include "include/AmptekSpectrum.h"
