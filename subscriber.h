
#pragma once

#include "reader.h"

	namespace CommLayer {

		template <typename T>

			class TSubscriber {

				private:
					typedef std::function<void(T&data)> callback;
        	                        T& create_data_reader_(generic_reader<T> *);
					DDSDomainParticipant * participant;
					DDSTopic *topic;
					DDSSubscriber *subscriber;
					DDSDataReader * data_reader;
					generic_reader<T> *listener;
					std::string topicName;
					uint8_t domainId;

				public:
					T *instance;
					TSubscriber(std::string topic, uint8_t id){
						domainId = id;
						topicName = topic;
						
					}

					bool create_participant();
					bool create_topic();
					bool cleanup_dds();

					T & create_subscriber(callback);
                                        T & create_subscriber(void (*funcptr)(T &));

					~TSubscriber() {
						delete listener;
						cleanup_dds();
					}
			};

		template<typename T> bool TSubscriber<T>::create_participant(){

			assert(participant==nullptr);
			bool ret = true;	
			participant = DDSDomainParticipantFactory::get_instance()->
				create_participant(
						domainId,
						DDS_PARTICIPANT_QOS_DEFAULT,
						nullptr,   /* Listener */
						DDS_STATUS_MASK_NONE);
			if(participant==nullptr) {
				std::cerr <<"Error Creating Domain participant"<<std::endl;
				ret = false;
			}


			return ret;
		}

		template <typename T> bool TSubscriber<T> ::create_topic(){

			DDS_ReturnCode_t rc;
			bool ret =true;

			assert(topic==nullptr);

			rc = T::TypeSupport::register_type(participant, T::TypeSupport::get_type_name());
			if(rc !=DDS_RETCODE_OK) {
				std::cerr << "Unable to Register Data Type" << T::TypeSupport::get_type_name()
					<<rc<<std::endl;
				ret = false;

			}
			else {
				
				topic = participant->create_topic(topicName.c_str(), T::TypeSupport::get_type_name(),
						DDS_TOPIC_QOS_DEFAULT,  nullptr, /* listener */
						DDS_STATUS_MASK_NONE);
				if(!topic) {
					ret = false;
					std::cerr<<"Failed to Create Topic in Subscriber"<<std::endl;
				}
			}

			return ret;
		}


                template<typename T> T& TSubscriber<T> ::create_subscriber( void (*funcptr)(T &) ){
                    	assert(subscriber==nullptr);
			subscriber = participant->create_subscriber (DDS_SUBSCRIBER_QOS_DEFAULT,
					nullptr,
					DDS_STATUS_MASK_NONE);


                        try {
        			listener = new generic_reader<T>(funcptr);
                        } catch ( ... ) {
				std::cerr << "Cannot create Data listener"<< std::endl;
                        }
                        return create_data_reader_(listener);
                }


		template<typename T> T& TSubscriber<T> ::create_subscriber( callback funcptr ){

			assert(subscriber==nullptr);
			subscriber = participant->create_subscriber (DDS_SUBSCRIBER_QOS_DEFAULT,
					nullptr,
					DDS_STATUS_MASK_NONE);

                        try {
        			listener = new generic_reader<T>(funcptr);
                        } catch ( ... ) {
				std::cerr << "Cannot create Data listener"<< std::endl;
                        }
                        return create_data_reader_(listener);
                }

                
        	template<typename T> T& TSubscriber<T> ::create_data_reader_ (generic_reader<T> *listener) {
			data_reader = subscriber->create_datareader(topic, 
					DDS_DATAREADER_QOS_DEFAULT,
					listener,
					DDS_STATUS_MASK_ALL);

			
			if(data_reader == nullptr) {
				std::cerr << "Cannot create Data Reader"<< std::endl;
			}

			return listener->instance;
		}
        }

template<typename T> bool ssic::comms::TSubscriber<T>::cleanup_dds() {
	bool ret = true;
	DDS_ReturnCode_t         rt;
	if (participant != nullptr) {
	        rt = participant->delete_contained_entities();
	        if (rt != DDS_RETCODE_OK) {
	            std::cerr << "Deletion failed." << std::endl;
	            ret = false;
	        }
	        rt = DDSDomainParticipantFactory::get_instance()->delete_participant(participant);
	        if (rt != DDS_RETCODE_OK) {
	            std::cerr << "Deletion failed." << std::endl;
	            ret = false;
	        }
	    }

    return ret;
}

