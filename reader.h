


#pragma once

#include <iostream>
#include <string>
#include "ndds/ndds_cpp.h"
#include "control.h"
#include "controlSupport.h"
#include <cassert>
#include <functional>

	namespace CommLayer {

		template<typename T>
			class TReader : public DDSDataReaderListener {

				private:
                                        void (*private_callback)(T & data);
                                        typedef std::function<void(T&data)> callback;
                                        callback private_f_;
				public:

					T instance; /* Class Instance which holds data */
					TReader(callback funcptr) : \
                                                        private_callback(nullptr),
                                                        private_f_ (nullptr)  {
						private_f_ = funcptr;
					}

					TReader(void (*funcptr)(T &)): \
                                                        private_callback(nullptr),
                                                        private_f_ (nullptr)  {
						private_callback = funcptr;
					}

					~TReader(){
						
					}

					void on_data_available(DDSDataReader* reader); 
			};  // End of class
	}



template<typename T> void ssic::comms::TReader<T>::on_data_available(DDSDataReader* reader){

	DDS_ReturnCode_t rc;
	typename T::DataReader *generic_datareader;

	DDS_SampleInfo info;
	DDS_ReturnCode_t  rt;

	for(;;) {
		generic_datareader = T::DataReader::narrow(reader);
		if(generic_datareader == nullptr) {
			std::cerr << "Failed to typecast Data(IDL)_FrameDataReader"<<std::endl;
			return;
		}

		rt = generic_datareader->read_next_sample(instance, info);

		if(rt == DDS_RETCODE_NO_DATA) {
			break;

		}
		else if( rt !=DDS_RETCODE_OK) {
			std::cerr << "Unable to Read Data" <<std::endl;
			return;
		}

		if(info.valid_data) {
                        if (private_callback)
                            private_callback(instance);
                        else
                            private_f_(instance);
		}
	}

}
