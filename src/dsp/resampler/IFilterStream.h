//
// Created by TheDaChicken on 9/2/2025.
//

#ifndef IFILTER_H
#define IFILTER_H

#include <memory>

#include "IFilterBlock.h"
#include "dsp/utils/HistoryBuffer.h"

class IFilterStream
{
	public:
		virtual ~IFilterStream() = default;

		virtual void Reset() = 0;
		virtual void SetFilter(std::shared_ptr<IFilterBlock> filter) = 0;

		virtual unsigned int IProcess(void *output, const void *input,
		                              unsigned int len) = 0;
};

template<typename IN_T>
class FilterStream final : public IFilterStream
{
	public:
		struct StreamOut
		{
			void* data;
			int size;
		};

		void Reset()
		{
			history_.Reset();
		}

		void SetFilter(std::shared_ptr<IFilterBlock> filter)
		{
			filter_ = std::move(filter);
			history_.SetHistorySize(filter_->BlockSize());
			history_.SetBlockSize(4); // Arbitrary size
			history_.Reset();
		}

		unsigned int IProcess(void *output, const void *input,
		                      const unsigned int len) override
		{
			return Process(static_cast<const IN_T *>(input), static_cast<IN_T *>(output), len);
		}

		unsigned int Process(const IN_T *input, IN_T *output, const unsigned int len)
		{
			unsigned int i_out = 0, i_in = 0;

			while (i_in < len)
			{
				unsigned int left = std::min(len - i_in, history_.WriteAvailable());

				history_.Write(&input[i_in], left);
				i_in += left;

				// If we reach the end of the input, and we have not filled the buffer,
				// we can stop processing.
				if (history_.ReadAvailable() < filter_->BlockSize())
					break;

				int n_read;
				i_out += filter_->ProcessBlock(&output[i_out],
				                               history_.Get(),
				                               filter_->BlockSize(),
				                               n_read);

				history_.Consume(n_read);
			}

			return i_out;
		}

	private:
		HistoryBuffer<IN_T> history_;
		std::shared_ptr<IFilterBlock> filter_;
		std::vector<IN_T> buffer_;
};

#endif //IFILTER_H
