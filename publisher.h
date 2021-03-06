

#pragma once

#include <iostream>
#include "ndds/ndds_cpp.h"
#include "control.h"
#include "controlSupport.h"
#include <cassert>

namespace CommLayer {

	template <typename T>

		class TPublisher {

			private:

				DDSDomainParticipant * participant;
				DDSTopic *topic;
				DDSPublisher *publisher;
				DDSDataWriter *datawriter;

				typename T::DataWriter * TDataWriter;

				std::string topicName;

				uint8_t domainId;

			public:

				T *instance;

				TPublisher(std::string topic, uint8_t id){
					domainId = id;
					topicName = topic;

				}
				bool create_participant();
				bool create_topic();
				bool create_datawriter();
				bool publish();
				bool cleanup_dds();
				
				~TPublisher() {
					cleanup_dds();
				}
				
		};
}




template<typename T> bool ssic::comms::TPublisher<T>::create_participant(){

	bool ret = true;
	assert(participant==nullptr);
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

	
	publisher = participant->create_publisher( /* create publisher from participant */
			DDS_PUBLISHER_QOS_DEFAULT,
			nullptr,
			DDS_STATUS_MASK_NONE);

	if(publisher == nullptr) {
		std::cerr <<" Failed to Create DDS Pub" <<std::endl;
		ret = false;

	}
	return ret;
}

template <typename T> bool ssic::comms::TPublisher<T> ::create_topic(){

	DDS_ReturnCode_t rc;
	bool ret = true;

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
		if(topic==nullptr) {
			std::cerr<<"Failed to Create Generic Publisher Topic"<<std::endl;
			ret = false;
		}
	}

	return ret;
}


template <typename T> bool ssic::comms::TPublisher<T> ::create_datawriter(){

	DDS_ReturnCode_t rc;
	bool ret =true;

	assert(datawriter==nullptr);
	datawriter = publisher->create_datawriter(topic, 
			DDS_DATAWRITER_QOS_DEFAULT,
			nullptr,
			DDS_STATUS_MASK_NONE);


	if(datawriter == nullptr) {
		std::cerr << "Failed to create DDS Writer"<<std::endl;
		ret = false;

	}
	
	/* Typecast generic DDS Writer to data class writer auto generated by rtiddsgen */

	TDataWriter = T::DataWriter::narrow(datawriter);
	if(TDataWriter ==  nullptr) {
		std::cerr << "Failed to typecast DataWriter"<<std::endl;
		ret = false;
	}

	assert(TDataWriter!=nullptr);

	instance = T::TypeSupport::create_data();
	if(instance==nullptr) {
		std::cerr << "Failed to allocate struct data defined in IDL "<<std::endl;
		ret = false;
	}


	return ret;
}

template<typename T> bool ssic::comms::TPublisher<T>::publish(){
	bool ret =true;	
	DDS_ReturnCode_t         rt;
	assert(TDataWriter!=nullptr);
	rt = TDataWriter->write(*instance, DDS_HANDLE_NIL);
	if(rt!=DDS_RETCODE_OK) {
		std::cerr << "TDataWriter error"<<std::endl;
		ret = false;
	}

	return ret;

}


template<typename T> bool ssic::comms::TPublisher<T>::cleanup_dds() {
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
#endif
