#pragma once

template <class _T>
class ChainedObject
{
public:
    ChainedObject(void);
    ~ChainedObject(void);

    inline _T* getNextObject() const { return m_nextobject; }
    inline _T* getPrevObject() const { return m_prevobject; }

    inline static _T* getFirstObject() { return s_firstobject; }

    template <class _COMP, typename _KEY>
    static _T* findObject(const _KEY& key, _COMP& comp = _COMP())
    {
        _T* object = s_firstobject;
        while (object) {
            if (comp(*object, key))
                return object;
            object = object->m_nextobject;
        }
        return 0;
    };

    template <class _FUNCTOR>
    static void doForEachObject(_FUNCTOR& func = _FUNCTOR())
    {
        _T* object = s_firstobject;
        while (object) {
            func(*object);
            object = object->m_nextobject;
        }
    };

protected:
    _T*        m_nextobject;
    _T*        m_prevobject;
    static _T* s_firstobject;

private:
    template <class _T2>
    ChainedObject<_T>& operator=(ChainedObject<_T2>&);

    template <class _T2>
    ChainedObject<_T>(const ChainedObject<_T2>&);
};

template <class _T>
_T* ChainedObject<_T>::s_firstobject = 0;

template <class _T>
ChainedObject<_T>::ChainedObject(void)
{
    m_nextobject = s_firstobject;
    m_prevobject = 0;

    if (s_firstobject)
        s_firstobject->m_prevobject = static_cast<_T*>(this);
    s_firstobject = static_cast<_T*>(this);
}

template <class _T>
ChainedObject<_T>::~ChainedObject(void)
{
    if (m_nextobject)
        m_nextobject->m_prevobject = this->m_prevobject;
    if (m_prevobject)
        m_prevobject->m_nextobject = this->m_nextobject;

    if (s_firstobject == static_cast<_T*>(this))
        s_firstobject = m_nextobject;
}
