/****************************************************************************
** Meta object code from reading C++ file 'streamthread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../includes/Qts/streamthread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'streamthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_StreamThread_t {
    QByteArrayData data[12];
    char stringdata[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_StreamThread_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_StreamThread_t qt_meta_stringdata_StreamThread = {
    {
QT_MOC_LITERAL(0, 0, 12), // "StreamThread"
QT_MOC_LITERAL(1, 13, 8), // "initBBox"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 10), // "aFrameDone"
QT_MOC_LITERAL(4, 34, 5), // "debug"
QT_MOC_LITERAL(5, 40, 11), // "const char*"
QT_MOC_LITERAL(6, 52, 3), // "msg"
QT_MOC_LITERAL(7, 56, 7), // "initSig"
QT_MOC_LITERAL(8, 64, 11), // "streamStart"
QT_MOC_LITERAL(9, 76, 12), // "std::string&"
QT_MOC_LITERAL(10, 89, 8), // "filename"
QT_MOC_LITERAL(11, 98, 4) // "init"

    },
    "StreamThread\0initBBox\0\0aFrameDone\0"
    "debug\0const char*\0msg\0initSig\0streamStart\0"
    "std::string&\0filename\0init"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StreamThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,
       4,    1,   46,    2, 0x06 /* Public */,
       7,    0,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   50,    2, 0x0a /* Public */,
      11,    0,   53,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Bool,

       0        // eod
};

void StreamThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StreamThread *_t = static_cast<StreamThread *>(_o);
        switch (_id) {
        case 0: _t->initBBox(); break;
        case 1: _t->aFrameDone(); break;
        case 2: _t->debug((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 3: _t->initSig(); break;
        case 4: _t->streamStart((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 5: { bool _r = _t->init();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (StreamThread::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StreamThread::initBBox)) {
                *result = 0;
            }
        }
        {
            typedef void (StreamThread::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StreamThread::aFrameDone)) {
                *result = 1;
            }
        }
        {
            typedef void (StreamThread::*_t)(const char * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StreamThread::debug)) {
                *result = 2;
            }
        }
        {
            typedef void (StreamThread::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StreamThread::initSig)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject StreamThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_StreamThread.data,
      qt_meta_data_StreamThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *StreamThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StreamThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_StreamThread.stringdata))
        return static_cast<void*>(const_cast< StreamThread*>(this));
    return QThread::qt_metacast(_clname);
}

int StreamThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void StreamThread::initBBox()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void StreamThread::aFrameDone()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void StreamThread::debug(const char * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void StreamThread::initSig()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
