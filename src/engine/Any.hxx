#ifndef __YACSANY_HXX__
#define __YACSANY_HXX__

#include "RefCounter.hxx"
#include "Exception.hxx"
#include "SharedPtr.hxx"

#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Any;
    class AtomAny;
    class TypeCode;
    class SeqAlloc;
    class ArrayAny;
    class SequenceAny;
    class TypeCodeArray;
    typedef void (*Deallocator)(void *);

    class StringOnHeap
    {
      friend class Any;
      friend class AtomAny;
      friend class ArrayAny;
    private:
      StringOnHeap(const char *val);
      StringOnHeap(const std::string& val);
      StringOnHeap(char *val, Deallocator deAlloc);
      bool operator ==(const StringOnHeap& other) const;
      StringOnHeap *deepCopy() const;
      const char *cStr() const { return _str; }
      ~StringOnHeap();
    private:
      char *_str;
      Deallocator _dealloc;
    };

    typedef SharedPtr<Any> AnyPtr;
    
    /*!
     * \brief: Interface for management of storage of data formated dynamically in its TypeCode.
     *         Warning virtual inheritance on Any daughter classes NOT supported.
     */
    class Any : public RefCounter
    {
      friend class SeqAlloc;
      friend class SequenceAny;
    public:
      const TypeCode *getType() const { return _type; }
      //for convenience methods
      virtual Any *clone() const = 0;
      virtual AnyPtr operator[](int i) const = 0;
      virtual bool operator ==(const Any& other) const = 0;
      virtual int getIntValue() const throw(Exception) = 0;
      virtual bool getBoolValue() const throw(Exception) = 0;
      virtual double getDoubleValue() const throw(Exception) = 0;
      virtual std::string getStringValue() const throw(Exception) = 0;
      //
    protected:
      virtual ~Any();
      Any(TypeCode* type);
      Any(const Any& other);
      virtual void putMyReprAtPlace(char *data) const = 0;
      static AnyPtr buildAnyFromCoarseData(char *data, TypeCode* type); //Factory Method
    protected:
      TypeCode* _type;
    };

    typedef SharedPtr<AtomAny> AtomAnyPtr;

    class AtomAny : public Any
    {
      friend class TypeCode;

      union ValueContainer
      {
        int _i;
        bool _b;
        double _d;
        StringOnHeap *_s;
      };
    public:
      Any *clone() const;
      template<class T>
      static AtomAny *New(T val) { return new AtomAny(val); }
      static AtomAny *New(char *val, Deallocator dealloc);
      AnyPtr operator[](int i) const;
      bool operator ==(const Any& other) const;
      int getIntValue() const throw(Exception);
      bool getBoolValue() const throw(Exception);
      double getDoubleValue() const throw(Exception);
      std::string getStringValue() const throw(Exception);
    protected:
      void putMyReprAtPlace(char *data) const;
      static void putReprAtPlace(char *data, const char *src, const TypeCode *type, bool deepCpy);
      static void destroyReprAtPlace(char *data, const TypeCode *type);
      static AnyPtr getOrBuildFromData(char *data, const TypeCode *type);
      static bool takeInChargeStorageOf(TypeCode *type);
    private:
      ~AtomAny();
      AtomAny(int val);
      AtomAny(bool val);
      AtomAny(double val);
      AtomAny(const char *val);
      AtomAny(const std::string& val);
      AtomAny(const AtomAny& other);
      AtomAny(char *data, TypeCode* type);
      AtomAny(char *val, Deallocator deAlloc);
    protected:
      ValueContainer _value;
    };
    
    class SeqAlloc 
    {
      friend class SequenceAny;
      
      char *_start;
      char *_finish;
      char *_endOfStorage;
      Deallocator _notStdDeAlloc;
      const unsigned int _sizeOf1Elm;
    private:
      SeqAlloc(const SeqAlloc& other);
      SeqAlloc(unsigned int sizeOf1Elm);
      ~SeqAlloc();
      void clear();
      void initCoarseMemory(char *mem, unsigned int size, Deallocator dealloc);
      void construct(char *pt, const Any *val);
      void construct(char *pt, const char *val, const TypeCode *tc, bool deepCpy);
      char *allocate(unsigned int nbOfByte);
      void destroy(char *pt, const TypeCode *tc);
      void deallocate(char *pt);
      unsigned int size() const;
    };
    
    class ComposedAny : public Any
    {
    protected:
      ComposedAny(const ComposedAny& other);
      ComposedAny(TypeCode* type);
    private://error methods called during incorrect runtime extraction
      int getIntValue() const throw(Exception);
      bool getBoolValue() const throw(Exception);
      double getDoubleValue() const throw(Exception);
      std::string getStringValue() const throw(Exception);
    };

    typedef SharedPtr<SequenceAny> SequenceAnyPtr;
    
    class SequenceAny : public ComposedAny
    {
      friend class TypeCodeSeq;
    public:
      void clear();
      void popBack();
      unsigned int size() const { return _alloc.size(); }
      void pushBack(const Any *elem);
      bool operator ==(const Any& other) const;
      void setEltAtRank(int i, const Any *elem);
      AnyPtr operator[](int i) const;
      Any *clone() const;
      template<class T>
      static SequenceAny *New(const std::vector<T>& vec);
      static SequenceAny *New(const TypeCode *typeOfContent);
      static SequenceAny *New(const TypeCode *typeOfContent, unsigned lgth);
      template<class T>
      static SequenceAny *New(T *val, unsigned int lgth, Deallocator deAlloc);
    protected:
      void putMyReprAtPlace(char *data) const;
      static void putReprAtPlace(char *data, const char *src, const TypeCode *type, bool deepCpy);
      static void destroyReprAtPlace(char *data, const TypeCode *type);
      static AnyPtr getOrBuildFromData(char *data, const TypeCode *type);
      static bool takeInChargeStorageOf(TypeCode *type);
    private:
      ~SequenceAny();
      SequenceAny(const SequenceAny& other);
      SequenceAny(const TypeCode *typeOfContent);
      SequenceAny(const TypeCode *typeOfContent, unsigned lgth);
      SequenceAny(int *val, unsigned int lgth, Deallocator deAlloc);
      SequenceAny(bool *val, unsigned int lgth, Deallocator deAlloc);
      SequenceAny(double *val, unsigned int lgth, Deallocator deAlloc);
      SequenceAny(const std::vector<int>& val);
      SequenceAny(const std::vector<bool>& val);
      SequenceAny(const std::vector<double>& val);
      SequenceAny(const std::vector<std::string>& val);
      void realloc(char *endOfCurrentAllocated, const Any *elem);
      char *performCpy(char *srcStart, char *srcFinish, char *destStart);
    protected:
      SeqAlloc _alloc;
    };
    
    typedef SharedPtr<ArrayAny> ArrayAnyPtr;

    class ArrayAny : public ComposedAny
    {
      friend class TypeCodeArray;
    public:
      bool operator ==(const Any& other) const;
      AnyPtr operator[](int i) const;
      Any *clone() const;
      template<class T>
      static ArrayAny *New(const std::vector<T>& vec);
      template<class T>
      static ArrayAny *New(const T *val, unsigned int lgth);
    protected:
      void putMyReprAtPlace(char *data) const;
      static void putReprAtPlace(char *data, const char *src, const TypeCodeArray *type, bool deepCpy);
      static void destroyReprAtPlace(char *data, const TypeCodeArray *type);
      static AnyPtr getOrBuildFromData(char *data, const TypeCodeArray *type);
      static bool takeInChargeStorageOf(TypeCode *type);
    private:
      ~ArrayAny();
      ArrayAny(char *data, TypeCodeArray * type);
      ArrayAny(const ArrayAny& other);
      ArrayAny(const int *val, unsigned int lgth);
      ArrayAny(const bool *val, unsigned int lgth);
      ArrayAny(const double *val, unsigned int lgth);
      ArrayAny(const std::vector<int>& val);
      ArrayAny(const std::vector<double>& val);
      ArrayAny(const std::vector<std::string>& val);
    protected:
      char *_data;
    };

    template<class T>
    SequenceAny *SequenceAny::New(T *val, unsigned int lgth, Deallocator deAlloc)
    {
      return new SequenceAny(val,lgth,deAlloc);
    }

    template<class T>
    SequenceAny *SequenceAny::New(const std::vector<T>& vec)
    {
      return new SequenceAny(vec);
    }

    template<class T>
    ArrayAny *ArrayAny::New(const std::vector<T>& vec)
    {
      return new ArrayAny(vec);
    }

    template<class T>
    ArrayAny *ArrayAny::New(const T *val, unsigned int lgth)
    {
      return new ArrayAny(val,lgth);
    }
  }
}

#endif
