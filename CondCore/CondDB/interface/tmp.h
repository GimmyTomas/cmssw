#ifndef CondCore_CondDB_tmp_h
#define CondCore_CondDB_tmp_h

#include "CondCore/DBCommon/interface/DbSession.h"
#include "CondCore/DBCommon/interface/TagMetadata.h"
#include "CondCore/IOVService/interface/IOVProxy.h"
#include "CondCore/IOVService/interface/IOVEditor.h"
#include "CondCore/CondDB/interface/Session.h"
#include "CondCore/CondDB/interface/Configuration.h"

namespace tmp {

    class Switch {
    public:
      bool open( const std::string& connectionString, bool readOnly=false  );
      
      bool isORA = false;
      cond::DbSession ORAImpl;
      new_impl::Session impl;
    };

    class Transaction {
    public:
      explicit Transaction( Switch& s );
      Transaction( const Transaction& rhs );
      Transaction& operator=( const Transaction& rhs );

      void start( bool readOnly=true );

      void commit();

      void rollback();

      bool isActive(); 
    private:
      Switch* m_switch;
    };

    class IOVProxy {
    public:
      class Iterator  : public std::iterator<std::input_iterator_tag, conddb::Iov_t> {
      public:
        //
        Iterator();
        explicit Iterator( new_impl::IOVProxy::Iterator impl );
        explicit Iterator( cond::IOVProxy::const_iterator impl );
        Iterator( const Iterator& rhs );

        //
        Iterator& operator=( const Iterator& rhs );

	//
        conddb::Iov_t operator*();

        //
        Iterator& operator++();
        Iterator operator++(int);

        //
        bool operator==( const Iterator& rhs ) const;
        bool operator!=( const Iterator& rhs ) const;

      private:
        new_impl::IOVProxy::Iterator m_impl;
        cond::IOVProxy::const_iterator m_ORAImpl;
        bool m_isORA = false;
      };

    public:
      explicit IOVProxy( const new_impl::IOVProxy& impl );
      explicit IOVProxy( cond::DbSession& ORASession );

      //
      IOVProxy( const IOVProxy& rhs );

      //
      IOVProxy& operator=( const IOVProxy& rhs );

      void load( const std::string& tag, bool full=false );

      void reload();

      void reset();

      std::string tag() const; 

      conddb::TimeType timeType() const;

      std::string payloadObjectType() const;

      conddb::Time_t endOfValidity() const;

      conddb::Time_t lastValidatedTime() const;

      Iterator begin() const;

      Iterator end() const;

      Iterator find(conddb::Time_t time);

      conddb::Iov_t getInterval( conddb::Time_t time );
    
      int size() const;

    private:
      new_impl::IOVProxy m_impl;
      cond::IOVProxy m_ORAImpl;
      std::string m_ORATag;
      bool m_isORA = false;
    };

    class IOVEditor {
    public:

      explicit IOVEditor( const new_impl::IOVEditor& impl );
      explicit IOVEditor( const cond::IOVEditor& ORAImpl );

      IOVEditor( const IOVEditor& rhs );

      //
      IOVEditor& operator=( const IOVEditor& rhs );

      void load( const std::string& tag );

      std::string tag() const;
      conddb::TimeType timeType() const;
      std::string payloadType() const;
      conddb::SynchronizationType synchronizationType() const;

      conddb::Time_t endOfValidity() const;
      void setEndOfValidity( conddb::Time_t validity );

      std::string description() const;
      void setDescription( const std::string& description );
      
      conddb::Time_t lastValidatedTime() const;
      void setLastValidatedTime( conddb::Time_t time );  

      void insert( conddb::Time_t since, const conddb::Hash& payloadHash, bool checkType=false );
      void insert( conddb::Time_t since, const conddb::Hash& payloadHash, const boost::posix_time::ptime& insertionTime, bool checkType=false ); 

      bool flush();
      bool flush( const boost::posix_time::ptime& operationTime );

    private:
      new_impl::IOVEditor m_impl;
      cond::IOVEditor m_ORAImpl;
      std::string m_ORATag;
      bool m_isORA = false;
    };

    class GTProxy {
    public:
      class Iterator  : public std::iterator<std::input_iterator_tag, conddb::GTEntry_t> {
      public:
	//
	Iterator();
	explicit Iterator( new_impl::GTProxy::Iterator impl );
	explicit Iterator( std::set<cond::TagMetadata>::const_iterator impl );
	Iterator( const Iterator& rhs );

	Iterator& operator=( const Iterator& rhs );

	conddb::GTEntry_t operator*();

	Iterator& operator++();
	Iterator operator++(int);

	bool operator==( const Iterator& rhs ) const;
	bool operator!=( const Iterator& rhs ) const;

      private:
        new_impl::GTProxy::Iterator m_impl;
        std::set<cond::TagMetadata>::const_iterator m_ORAImpl;
        bool m_isORA = false;
      };
    
    public:
      explicit GTProxy( const new_impl::GTProxy& impl );
      explicit GTProxy( const cond::DbSession& ORASession );

      GTProxy( const GTProxy& rhs );

      GTProxy& operator=( const GTProxy& rhs );
      
      void load( const std::string& gtName );

      void reload();

      void reset();

      std::string name() const; 

      conddb::Time_t validity() const;

      boost::posix_time::ptime snapshotTime() const;

      Iterator begin() const;

      Iterator end() const;
    
      int size() const;

    private:
      new_impl::GTProxy m_impl;
      cond::DbSession m_ORASession;
      std::set<cond::TagMetadata> m_ORAGTData;
      std::string m_ORAGTName;
      bool m_isORA = false;
    };


    class Session {
    public:
      Session();

      Session( const Session& rhs );

      virtual ~Session(){}

      Session& operator=( const Session& rhs );

      void open( const std::string& connectionString, bool readOnly=false );

      void close();

      conddb::Configuration& configuration();

      Transaction& transaction();

      IOVProxy readIov( const std::string& tag, bool full=false );//,const boost::posix_time::ptime& snapshottime )  

      bool existIov( const std::string& tag );

      template <typename T>
      IOVEditor createIov( const std::string& tag,conddb:: TimeType timeType, 
			   conddb::SynchronizationType synchronizationType=conddb::OFFLINE );
      IOVEditor createIov( const std::string& tag, conddb::TimeType timeType, const std::string& payloadType, 
			   conddb::SynchronizationType synchronizationType=conddb::OFFLINE );

      IOVEditor editIov( const std::string& tag );

      template <typename T> conddb::Hash storePayload( const T& payload, const boost::posix_time::ptime& creationTime );
      template <typename T> boost::shared_ptr<T> fetchPayload( const conddb::Hash& payloadHash );

      IOVProxy iovProxy();

      GTProxy readGlobalTag( const std::string& name );

    private:

      Switch m_switch;
      Transaction m_transaction;
    };

    template <typename T> inline IOVEditor Session::createIov( const std::string& tag, conddb::TimeType timeType, conddb::SynchronizationType synchronizationType ){
      return createIov( tag, timeType, conddb::demangledName( typeid(T) ), synchronizationType );
    }

    template <typename T> inline conddb::Hash Session::storePayload( const T& payload, const boost::posix_time::ptime& creationTime ){
      conddb::Hash hashOrToken("");
      if( m_switch.isORA ){
	hashOrToken = m_switch.ORAImpl.storeObject( &payload, conddb::demangledName(typeid(payload)) );
      } else {
	hashOrToken = m_switch.impl.storePayload( payload, creationTime );
      }
      return hashOrToken;
    }

    template <typename T> inline boost::shared_ptr<T> Session::fetchPayload( const conddb::Hash& payloadHash ){
      boost::shared_ptr<T> obj;
      if( m_switch.isORA ) {
	obj = m_switch.ORAImpl.getTypedObject<T>( payloadHash );
      } else {
	obj = m_switch.impl.fetchPayload<T>( payloadHash );
      }
      return obj;
    }

  /**
  template <typename T> class PayloadProxy {
  public:
    explicit PayloadProxy( Session& session );

    PayloadProxy( const PayloadProxy& rhs );

    PayloadProxy& operator=( const PayloadProxy& rhs );

    void load( const std::string& tag );

    void reload();

    void reset();

    boost::shared_ptr<T> get( conddb::Time_t targetTime );

  private:
    
    Session m_session;
    IOVProxy m_iov;
    conddb::Iov_t m_current;
    boost::shared_ptr<T> m_cache;
  };

  template <typename T> inline PayloadProxy<T>::PayloadProxy( Session& session ):
    m_session( session ),
    m_iov( session.iovProxy() ),
    m_current(),
    m_cache(){
    m_current.clear();
  }

  template <typename T> inline PayloadProxy<T>::PayloadProxy( const PayloadProxy& rhs ):
    m_session( rhs.session ),
    m_iov( rhs.m_iov ),
    m_current( rhs.m_current ),
    m_cache( rhs.m_cache ){
  }

  template <typename T> inline PayloadProxy<T>& PayloadProxy<T>::operator=( const PayloadProxy& rhs ){
    m_session = rhs.session;
    m_iov = rhs.m_iov;
    m_current = rhs.m_current;
    m_cache = rhs.m_cache;
    return *this;
  }

  template <typename T> inline void PayloadProxy<T>::load( const std::string& tag ){
    m_current.clear();
    m_cache.reset();
    m_iov.load( tag );
    std::string payloadType = m_iov.payloadObjectType();
    if( m_iov.payloadObjectType() != conddb::demangledName( typeid(T) ) ) {
      reset();      
      conddb::throwException("Type mismatch: type "+payloadType+
			     "defined for tag "+tag+" is different from the target type.",
			     "PayloadProxy::load");
    }
  }

  template <typename T> inline void PayloadProxy<T>::reload(){
    m_current.clear();
    m_cache.reset();
    m_iov.reload();
  }

  template <typename T> inline void PayloadProxy<T>::reset(){
    m_iov.reset();
    m_current.clear();
    m_cache.reset();
  }

  template <typename T> inline boost::shared_ptr<T> PayloadProxy<T>::get( conddb::Time_t targetTime ){
    //  check if the current iov loaded is the good one...
    if( targetTime < m_current.since || targetTime > m_current.till ){

      // a new payload is required!
      auto iIov = m_iov.find( targetTime );
      if(iIov == m_iov.end() ) conddb::throwException("No iov available for the specified target time.","PayloadProxy::get");
      m_current = *iIov;

      // finally load the new payload into the cache
      m_cache = m_session.fetchPayload<T>( m_current.payloadId );
    } 
    return m_cache;
  }
  **/

}

#endif
