# This file was automatically generated by SWIG (http://www.swig.org).
# Version 3.0.8
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.





from sys import version_info
if version_info >= (2, 6, 0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_AmptekHardwareInterface', [dirname(__file__)])
        except ImportError:
            import _AmptekHardwareInterface
            return _AmptekHardwareInterface
        if fp is not None:
            try:
                _mod = imp.load_module('_AmptekHardwareInterface', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _AmptekHardwareInterface = swig_import_helper()
    del swig_import_helper
else:
    import _AmptekHardwareInterface
del version_info
try:
    _swig_property = property
except NameError:
    pass  # Python < 2.2 doesn't have 'property'.


def _swig_setattr_nondynamic(self, class_type, name, value, static=1):
    if (name == "thisown"):
        return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name, None)
    if method:
        return method(self, value)
    if (not static):
        object.__setattr__(self, name, value)
    else:
        raise AttributeError("You cannot add attributes to %s" % self)


def _swig_setattr(self, class_type, name, value):
    return _swig_setattr_nondynamic(self, class_type, name, value, 0)


def _swig_getattr_nondynamic(self, class_type, name, static=1):
    if (name == "thisown"):
        return self.this.own()
    method = class_type.__swig_getmethods__.get(name, None)
    if method:
        return method(self)
    if (not static):
        return object.__getattr__(self, name)
    else:
        raise AttributeError(name)

def _swig_getattr(self, class_type, name):
    return _swig_getattr_nondynamic(self, class_type, name, 0)


def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object:
        pass
    _newclass = 0



def _swig_setattr_nondynamic_method(set):
    def set_attr(self, name, value):
        if (name == "thisown"):
            return self.this.own(value)
        if hasattr(self, name) or (name == "this"):
            set(self, name, value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


class SwigPyIterator(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    __swig_destroy__ = _AmptekHardwareInterface.delete_SwigPyIterator
    __del__ = lambda self: None

    def value(self):
        return _AmptekHardwareInterface.SwigPyIterator_value(self)

    def incr(self, n=1):
        return _AmptekHardwareInterface.SwigPyIterator_incr(self, n)

    def decr(self, n=1):
        return _AmptekHardwareInterface.SwigPyIterator_decr(self, n)

    def distance(self, x):
        return _AmptekHardwareInterface.SwigPyIterator_distance(self, x)

    def equal(self, x):
        return _AmptekHardwareInterface.SwigPyIterator_equal(self, x)

    def copy(self):
        return _AmptekHardwareInterface.SwigPyIterator_copy(self)

    def next(self):
        return _AmptekHardwareInterface.SwigPyIterator_next(self)

    def __next__(self):
        return _AmptekHardwareInterface.SwigPyIterator___next__(self)

    def previous(self):
        return _AmptekHardwareInterface.SwigPyIterator_previous(self)

    def advance(self, n):
        return _AmptekHardwareInterface.SwigPyIterator_advance(self, n)

    def __eq__(self, x):
        return _AmptekHardwareInterface.SwigPyIterator___eq__(self, x)

    def __ne__(self, x):
        return _AmptekHardwareInterface.SwigPyIterator___ne__(self, x)

    def __iadd__(self, n):
        return _AmptekHardwareInterface.SwigPyIterator___iadd__(self, n)

    def __isub__(self, n):
        return _AmptekHardwareInterface.SwigPyIterator___isub__(self, n)

    def __add__(self, n):
        return _AmptekHardwareInterface.SwigPyIterator___add__(self, n)

    def __sub__(self, *args):
        return _AmptekHardwareInterface.SwigPyIterator___sub__(self, *args)
    def __iter__(self):
        return self
SwigPyIterator_swigregister = _AmptekHardwareInterface.SwigPyIterator_swigregister
SwigPyIterator_swigregister(SwigPyIterator)

class IntVector(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr

    def iterator(self):
        return _AmptekHardwareInterface.IntVector_iterator(self)
    def __iter__(self):
        return self.iterator()

    def __nonzero__(self):
        return _AmptekHardwareInterface.IntVector___nonzero__(self)

    def __bool__(self):
        return _AmptekHardwareInterface.IntVector___bool__(self)

    def __len__(self):
        return _AmptekHardwareInterface.IntVector___len__(self)

    def __getslice__(self, i, j):
        return _AmptekHardwareInterface.IntVector___getslice__(self, i, j)

    def __setslice__(self, *args):
        return _AmptekHardwareInterface.IntVector___setslice__(self, *args)

    def __delslice__(self, i, j):
        return _AmptekHardwareInterface.IntVector___delslice__(self, i, j)

    def __delitem__(self, *args):
        return _AmptekHardwareInterface.IntVector___delitem__(self, *args)

    def __getitem__(self, *args):
        return _AmptekHardwareInterface.IntVector___getitem__(self, *args)

    def __setitem__(self, *args):
        return _AmptekHardwareInterface.IntVector___setitem__(self, *args)

    def pop(self):
        return _AmptekHardwareInterface.IntVector_pop(self)

    def append(self, x):
        return _AmptekHardwareInterface.IntVector_append(self, x)

    def empty(self):
        return _AmptekHardwareInterface.IntVector_empty(self)

    def size(self):
        return _AmptekHardwareInterface.IntVector_size(self)

    def swap(self, v):
        return _AmptekHardwareInterface.IntVector_swap(self, v)

    def begin(self):
        return _AmptekHardwareInterface.IntVector_begin(self)

    def end(self):
        return _AmptekHardwareInterface.IntVector_end(self)

    def rbegin(self):
        return _AmptekHardwareInterface.IntVector_rbegin(self)

    def rend(self):
        return _AmptekHardwareInterface.IntVector_rend(self)

    def clear(self):
        return _AmptekHardwareInterface.IntVector_clear(self)

    def get_allocator(self):
        return _AmptekHardwareInterface.IntVector_get_allocator(self)

    def pop_back(self):
        return _AmptekHardwareInterface.IntVector_pop_back(self)

    def erase(self, *args):
        return _AmptekHardwareInterface.IntVector_erase(self, *args)

    def __init__(self, *args):
        this = _AmptekHardwareInterface.new_IntVector(*args)
        try:
            self.this.append(this)
        except Exception:
            self.this = this

    def push_back(self, x):
        return _AmptekHardwareInterface.IntVector_push_back(self, x)

    def front(self):
        return _AmptekHardwareInterface.IntVector_front(self)

    def back(self):
        return _AmptekHardwareInterface.IntVector_back(self)

    def assign(self, n, x):
        return _AmptekHardwareInterface.IntVector_assign(self, n, x)

    def resize(self, *args):
        return _AmptekHardwareInterface.IntVector_resize(self, *args)

    def insert(self, *args):
        return _AmptekHardwareInterface.IntVector_insert(self, *args)

    def reserve(self, n):
        return _AmptekHardwareInterface.IntVector_reserve(self, n)

    def capacity(self):
        return _AmptekHardwareInterface.IntVector_capacity(self)
    __swig_destroy__ = _AmptekHardwareInterface.delete_IntVector
    __del__ = lambda self: None
IntVector_swigregister = _AmptekHardwareInterface.IntVector_swigregister
IntVector_swigregister(IntVector)

class StringVector(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr

    def iterator(self):
        return _AmptekHardwareInterface.StringVector_iterator(self)
    def __iter__(self):
        return self.iterator()

    def __nonzero__(self):
        return _AmptekHardwareInterface.StringVector___nonzero__(self)

    def __bool__(self):
        return _AmptekHardwareInterface.StringVector___bool__(self)

    def __len__(self):
        return _AmptekHardwareInterface.StringVector___len__(self)

    def __getslice__(self, i, j):
        return _AmptekHardwareInterface.StringVector___getslice__(self, i, j)

    def __setslice__(self, *args):
        return _AmptekHardwareInterface.StringVector___setslice__(self, *args)

    def __delslice__(self, i, j):
        return _AmptekHardwareInterface.StringVector___delslice__(self, i, j)

    def __delitem__(self, *args):
        return _AmptekHardwareInterface.StringVector___delitem__(self, *args)

    def __getitem__(self, *args):
        return _AmptekHardwareInterface.StringVector___getitem__(self, *args)

    def __setitem__(self, *args):
        return _AmptekHardwareInterface.StringVector___setitem__(self, *args)

    def pop(self):
        return _AmptekHardwareInterface.StringVector_pop(self)

    def append(self, x):
        return _AmptekHardwareInterface.StringVector_append(self, x)

    def empty(self):
        return _AmptekHardwareInterface.StringVector_empty(self)

    def size(self):
        return _AmptekHardwareInterface.StringVector_size(self)

    def swap(self, v):
        return _AmptekHardwareInterface.StringVector_swap(self, v)

    def begin(self):
        return _AmptekHardwareInterface.StringVector_begin(self)

    def end(self):
        return _AmptekHardwareInterface.StringVector_end(self)

    def rbegin(self):
        return _AmptekHardwareInterface.StringVector_rbegin(self)

    def rend(self):
        return _AmptekHardwareInterface.StringVector_rend(self)

    def clear(self):
        return _AmptekHardwareInterface.StringVector_clear(self)

    def get_allocator(self):
        return _AmptekHardwareInterface.StringVector_get_allocator(self)

    def pop_back(self):
        return _AmptekHardwareInterface.StringVector_pop_back(self)

    def erase(self, *args):
        return _AmptekHardwareInterface.StringVector_erase(self, *args)

    def __init__(self, *args):
        this = _AmptekHardwareInterface.new_StringVector(*args)
        try:
            self.this.append(this)
        except Exception:
            self.this = this

    def push_back(self, x):
        return _AmptekHardwareInterface.StringVector_push_back(self, x)

    def front(self):
        return _AmptekHardwareInterface.StringVector_front(self)

    def back(self):
        return _AmptekHardwareInterface.StringVector_back(self)

    def assign(self, n, x):
        return _AmptekHardwareInterface.StringVector_assign(self, n, x)

    def resize(self, *args):
        return _AmptekHardwareInterface.StringVector_resize(self, *args)

    def insert(self, *args):
        return _AmptekHardwareInterface.StringVector_insert(self, *args)

    def reserve(self, n):
        return _AmptekHardwareInterface.StringVector_reserve(self, n)

    def capacity(self):
        return _AmptekHardwareInterface.StringVector_capacity(self)
    __swig_destroy__ = _AmptekHardwareInterface.delete_StringVector
    __del__ = lambda self: None
StringVector_swigregister = _AmptekHardwareInterface.StringVector_swigregister
StringVector_swigregister(StringVector)

class DoubleVector(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr

    def iterator(self):
        return _AmptekHardwareInterface.DoubleVector_iterator(self)
    def __iter__(self):
        return self.iterator()

    def __nonzero__(self):
        return _AmptekHardwareInterface.DoubleVector___nonzero__(self)

    def __bool__(self):
        return _AmptekHardwareInterface.DoubleVector___bool__(self)

    def __len__(self):
        return _AmptekHardwareInterface.DoubleVector___len__(self)

    def __getslice__(self, i, j):
        return _AmptekHardwareInterface.DoubleVector___getslice__(self, i, j)

    def __setslice__(self, *args):
        return _AmptekHardwareInterface.DoubleVector___setslice__(self, *args)

    def __delslice__(self, i, j):
        return _AmptekHardwareInterface.DoubleVector___delslice__(self, i, j)

    def __delitem__(self, *args):
        return _AmptekHardwareInterface.DoubleVector___delitem__(self, *args)

    def __getitem__(self, *args):
        return _AmptekHardwareInterface.DoubleVector___getitem__(self, *args)

    def __setitem__(self, *args):
        return _AmptekHardwareInterface.DoubleVector___setitem__(self, *args)

    def pop(self):
        return _AmptekHardwareInterface.DoubleVector_pop(self)

    def append(self, x):
        return _AmptekHardwareInterface.DoubleVector_append(self, x)

    def empty(self):
        return _AmptekHardwareInterface.DoubleVector_empty(self)

    def size(self):
        return _AmptekHardwareInterface.DoubleVector_size(self)

    def swap(self, v):
        return _AmptekHardwareInterface.DoubleVector_swap(self, v)

    def begin(self):
        return _AmptekHardwareInterface.DoubleVector_begin(self)

    def end(self):
        return _AmptekHardwareInterface.DoubleVector_end(self)

    def rbegin(self):
        return _AmptekHardwareInterface.DoubleVector_rbegin(self)

    def rend(self):
        return _AmptekHardwareInterface.DoubleVector_rend(self)

    def clear(self):
        return _AmptekHardwareInterface.DoubleVector_clear(self)

    def get_allocator(self):
        return _AmptekHardwareInterface.DoubleVector_get_allocator(self)

    def pop_back(self):
        return _AmptekHardwareInterface.DoubleVector_pop_back(self)

    def erase(self, *args):
        return _AmptekHardwareInterface.DoubleVector_erase(self, *args)

    def __init__(self, *args):
        this = _AmptekHardwareInterface.new_DoubleVector(*args)
        try:
            self.this.append(this)
        except Exception:
            self.this = this

    def push_back(self, x):
        return _AmptekHardwareInterface.DoubleVector_push_back(self, x)

    def front(self):
        return _AmptekHardwareInterface.DoubleVector_front(self)

    def back(self):
        return _AmptekHardwareInterface.DoubleVector_back(self)

    def assign(self, n, x):
        return _AmptekHardwareInterface.DoubleVector_assign(self, n, x)

    def resize(self, *args):
        return _AmptekHardwareInterface.DoubleVector_resize(self, *args)

    def insert(self, *args):
        return _AmptekHardwareInterface.DoubleVector_insert(self, *args)

    def reserve(self, n):
        return _AmptekHardwareInterface.DoubleVector_reserve(self, n)

    def capacity(self):
        return _AmptekHardwareInterface.DoubleVector_capacity(self)
    __swig_destroy__ = _AmptekHardwareInterface.delete_DoubleVector
    __del__ = lambda self: None
DoubleVector_swigregister = _AmptekHardwareInterface.DoubleVector_swigregister
DoubleVector_swigregister(DoubleVector)

class UIntVector(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr

    def iterator(self):
        return _AmptekHardwareInterface.UIntVector_iterator(self)
    def __iter__(self):
        return self.iterator()

    def __nonzero__(self):
        return _AmptekHardwareInterface.UIntVector___nonzero__(self)

    def __bool__(self):
        return _AmptekHardwareInterface.UIntVector___bool__(self)

    def __len__(self):
        return _AmptekHardwareInterface.UIntVector___len__(self)

    def __getslice__(self, i, j):
        return _AmptekHardwareInterface.UIntVector___getslice__(self, i, j)

    def __setslice__(self, *args):
        return _AmptekHardwareInterface.UIntVector___setslice__(self, *args)

    def __delslice__(self, i, j):
        return _AmptekHardwareInterface.UIntVector___delslice__(self, i, j)

    def __delitem__(self, *args):
        return _AmptekHardwareInterface.UIntVector___delitem__(self, *args)

    def __getitem__(self, *args):
        return _AmptekHardwareInterface.UIntVector___getitem__(self, *args)

    def __setitem__(self, *args):
        return _AmptekHardwareInterface.UIntVector___setitem__(self, *args)

    def pop(self):
        return _AmptekHardwareInterface.UIntVector_pop(self)

    def append(self, x):
        return _AmptekHardwareInterface.UIntVector_append(self, x)

    def empty(self):
        return _AmptekHardwareInterface.UIntVector_empty(self)

    def size(self):
        return _AmptekHardwareInterface.UIntVector_size(self)

    def swap(self, v):
        return _AmptekHardwareInterface.UIntVector_swap(self, v)

    def begin(self):
        return _AmptekHardwareInterface.UIntVector_begin(self)

    def end(self):
        return _AmptekHardwareInterface.UIntVector_end(self)

    def rbegin(self):
        return _AmptekHardwareInterface.UIntVector_rbegin(self)

    def rend(self):
        return _AmptekHardwareInterface.UIntVector_rend(self)

    def clear(self):
        return _AmptekHardwareInterface.UIntVector_clear(self)

    def get_allocator(self):
        return _AmptekHardwareInterface.UIntVector_get_allocator(self)

    def pop_back(self):
        return _AmptekHardwareInterface.UIntVector_pop_back(self)

    def erase(self, *args):
        return _AmptekHardwareInterface.UIntVector_erase(self, *args)

    def __init__(self, *args):
        this = _AmptekHardwareInterface.new_UIntVector(*args)
        try:
            self.this.append(this)
        except Exception:
            self.this = this

    def push_back(self, x):
        return _AmptekHardwareInterface.UIntVector_push_back(self, x)

    def front(self):
        return _AmptekHardwareInterface.UIntVector_front(self)

    def back(self):
        return _AmptekHardwareInterface.UIntVector_back(self)

    def assign(self, n, x):
        return _AmptekHardwareInterface.UIntVector_assign(self, n, x)

    def resize(self, *args):
        return _AmptekHardwareInterface.UIntVector_resize(self, *args)

    def insert(self, *args):
        return _AmptekHardwareInterface.UIntVector_insert(self, *args)

    def reserve(self, n):
        return _AmptekHardwareInterface.UIntVector_reserve(self, n)

    def capacity(self):
        return _AmptekHardwareInterface.UIntVector_capacity(self)
    __swig_destroy__ = _AmptekHardwareInterface.delete_UIntVector
    __del__ = lambda self: None
UIntVector_swigregister = _AmptekHardwareInterface.UIntVector_swigregister
UIntVector_swigregister(UIntVector)


_AmptekHardwareInterface.STATUS_SIZE_swigconstant(_AmptekHardwareInterface)
STATUS_SIZE = _AmptekHardwareInterface.STATUS_SIZE

_AmptekHardwareInterface.SPECLEN_swigconstant(_AmptekHardwareInterface)
SPECLEN = _AmptekHardwareInterface.SPECLEN

_AmptekHardwareInterface.NOT_CONNECTED_swigconstant(_AmptekHardwareInterface)
NOT_CONNECTED = _AmptekHardwareInterface.NOT_CONNECTED

_AmptekHardwareInterface.ON_swigconstant(_AmptekHardwareInterface)
ON = _AmptekHardwareInterface.ON

_AmptekHardwareInterface.RECORDING_swigconstant(_AmptekHardwareInterface)
RECORDING = _AmptekHardwareInterface.RECORDING

_AmptekHardwareInterface.ERROR_swigconstant(_AmptekHardwareInterface)
ERROR = _AmptekHardwareInterface.ERROR
class AmptekHardwareInterface(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr

    def __init__(self):
        this = _AmptekHardwareInterface.new_AmptekHardwareInterface()
        try:
            self.this.append(this)
        except Exception:
            self.this = this
    __swig_destroy__ = _AmptekHardwareInterface.delete_AmptekHardwareInterface
    __del__ = lambda self: None

    def connectUSB(self, serialNb):
        return _AmptekHardwareInterface.AmptekHardwareInterface_connectUSB(self, serialNb)

    def connectSimulator(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_connectSimulator(self)

    def connectUDP(self, hostname, port, timeout):
        return _AmptekHardwareInterface.AmptekHardwareInterface_connectUDP(self, hostname, port, timeout)

    def readSpectrum(self, max_age_ms=-1):
        return _AmptekHardwareInterface.AmptekHardwareInterface_readSpectrum(self, max_age_ms)

    def readSpectrumAndStatus(self, statusbuffer, max_age_ms=-1):
        return _AmptekHardwareInterface.AmptekHardwareInterface_readSpectrumAndStatus(self, statusbuffer, max_age_ms)

    def Enable(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_Enable(self)

    def Disable(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_Disable(self)

    def Ping(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_Ping(self)

    def ClearSpectrum(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_ClearSpectrum(self)

    def SetPresetAccumulationTime(self, t):
        return _AmptekHardwareInterface.AmptekHardwareInterface_SetPresetAccumulationTime(self, t)

    def SetPresetRealTime(self, t):
        return _AmptekHardwareInterface.AmptekHardwareInterface_SetPresetRealTime(self, t)

    def SetPresetCounts(self, c):
        return _AmptekHardwareInterface.AmptekHardwareInterface_SetPresetCounts(self, c)

    def SetTextConfiguration(self, commands):
        return _AmptekHardwareInterface.AmptekHardwareInterface_SetTextConfiguration(self, commands)

    def UpdateStatus(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_UpdateStatus(self)

    def EnableListMode(self, targetfile):
        return _AmptekHardwareInterface.AmptekHardwareInterface_EnableListMode(self, targetfile)

    def ResetListModeTimer(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_ResetListModeTimer(self)

    def DisableListMode(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_DisableListMode(self)

    def startBuffering(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_startBuffering(self)

    def stopBuffering(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_stopBuffering(self)

    def GetBuffered(self, id):
        return _AmptekHardwareInterface.AmptekHardwareInterface_GetBuffered(self, id)

    def StartCommtestStreaming(self, min_channel, max_channel, increment, rate):
        return _AmptekHardwareInterface.AmptekHardwareInterface_StartCommtestStreaming(self, min_channel, max_channel, increment, rate)

    def StopCommtestStreaming(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_StopCommtestStreaming(self)

    def FastCount(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_FastCount(self, max_age_ms)

    def SlowCount(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_SlowCount(self, max_age_ms)

    def DeadTime(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_DeadTime(self, max_age_ms)

    def AccTime(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_AccTime(self, max_age_ms)

    def RealTime(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_RealTime(self, max_age_ms)

    def FirmwareMajor(self, max_age_ms=1000000):
        return _AmptekHardwareInterface.AmptekHardwareInterface_FirmwareMajor(self, max_age_ms)

    def FirmwareMinor(self, max_age_ms=1000000):
        return _AmptekHardwareInterface.AmptekHardwareInterface_FirmwareMinor(self, max_age_ms)

    def FirmwareBuild(self, max_age_ms=1000000):
        return _AmptekHardwareInterface.AmptekHardwareInterface_FirmwareBuild(self, max_age_ms)

    def FpgaMajor(self, max_age_ms=1000000):
        return _AmptekHardwareInterface.AmptekHardwareInterface_FpgaMajor(self, max_age_ms)

    def FpgaMinor(self, max_age_ms=1000000):
        return _AmptekHardwareInterface.AmptekHardwareInterface_FpgaMinor(self, max_age_ms)

    def SerialNb(self, max_age_ms=1000000):
        return _AmptekHardwareInterface.AmptekHardwareInterface_SerialNb(self, max_age_ms)

    def HighVoltage(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_HighVoltage(self, max_age_ms)

    def DetectorTemp(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_DetectorTemp(self, max_age_ms)

    def BoardTemp(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_BoardTemp(self, max_age_ms)

    def IsPresetTimeReached(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_IsPresetTimeReached(self, max_age_ms)

    def IsEnabled(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_IsEnabled(self, max_age_ms)

    def IsPresetCountReached(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_IsPresetCountReached(self, max_age_ms)

    def IsGated(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_IsGated(self, max_age_ms)

    def FpgaClock(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_FpgaClock(self, max_age_ms)

    def DeviceType(self, max_age_ms=1000000):
        return _AmptekHardwareInterface.AmptekHardwareInterface_DeviceType(self, max_age_ms)

    def TecVoltage(self, max_age_ms=100):
        return _AmptekHardwareInterface.AmptekHardwareInterface_TecVoltage(self, max_age_ms)

    def GetSpectrum(self, *args):
        return _AmptekHardwareInterface.AmptekHardwareInterface_GetSpectrum(self, *args)

    def GetTextConfiguration(self, commands):
        return _AmptekHardwareInterface.AmptekHardwareInterface_GetTextConfiguration(self, commands)

    def GetState(self):
        return _AmptekHardwareInterface.AmptekHardwareInterface_GetState(self)
AmptekHardwareInterface_swigregister = _AmptekHardwareInterface.AmptekHardwareInterface_swigregister
AmptekHardwareInterface_swigregister(AmptekHardwareInterface)



