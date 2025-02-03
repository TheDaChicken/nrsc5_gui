//
// Created by TheDaChicken on 8/14/2024.
//

#ifndef NRSC5_GUI_LIB_UTILS_DATABUFFER_H_
#define NRSC5_GUI_LIB_UTILS_DATABUFFER_H_

template<typename T>
class DataBuffer
{
	public:
		DataBuffer() = default;

		DataBuffer(const DataBuffer &other)
		{
			Resize(other.Size());
			memcpy(Data(), other.Data(), Size() * sizeof(T));
		}

		explicit DataBuffer(std::size_t n)
		{
			Resize(n);
		}

		explicit DataBuffer(T *ptr, const std::size_t n)
			: size_(n), capacity_(n), buffer_(ptr), ownership(false)
		{
		}

		DataBuffer &operator=(const DataBuffer &other)
		{
			if (this != &other)
			{
				Resize(other.Size());
				memcpy(Data(), other.Data(), Size() * sizeof(T));
			}
			return *this;
		}

		~DataBuffer()
		{
			Clear();
		}

		void Clear()
		{
			if (ownership && buffer_)
			{
				free(buffer_);
			}
			size_ = 0;
			capacity_ = 0;
		}

		int Resize(std::size_t n)
		{
			Reserve(n);
			size_ = n;
			return 0;
		}

		int Reserve(std::size_t n)
		{
			if (capacity_ > n)
			{
				return 0;
			}

			buffer_ = static_cast<T *>(realloc(buffer_, n * sizeof(T)));
			if (!buffer_)
			{
				return -1;
			}

			capacity_ = n;
			return 0;
		}

		[[nodiscard]] std::size_t Capacity() const
		{
			return capacity_;
		}

		[[nodiscard]] std::size_t Size() const
		{
			return size_;
		}

		T *Data()
		{
			return buffer_;
		}

		const T *Data() const
		{
			return buffer_;
		}

		T &operator[](int index)
		{
			return buffer_[index];
		}

		const T &operator[](int index) const
		{
			return buffer_[index];
		}

	private:
		std::size_t size_{0};
		std::size_t capacity_{0};

		T *buffer_{nullptr};
		bool ownership{true};
};

#endif //NRSC5_GUI_LIB_UTILS_DATABUFFER_H_
