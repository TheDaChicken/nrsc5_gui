//
// Created by TheDaChicken on 9/2/2025.
//

#ifndef IARBRESAMPLER_H
#define IARBRESAMPLER_H

class IFilterBlock
{
	public:
		explicit IFilterBlock()

		{
		}
		virtual ~IFilterBlock() = default;

		virtual int BlockSize() const = 0;
		virtual int ProcessBlock(void *output, const void *input, unsigned int n_to_read, int &consumed_samples) = 0;
};

#endif //IARBRESAMPLER_H
