/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataloader.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "ann/xtensor_lib.h"
#include "ann/dataset.h"

using namespace std;

template<typename DType, typename LType>
class DataLoader
{
public:
    class Iterator;

private:
    Dataset<DType, LType>* ptr_dataset; // pointer to dataset
    int batch_size; // size of each batch
    bool shuffle;
    bool drop_last;
    xt::xarray<int> indices; // array contains the data index
    int total_batch;
    int seed; // used to determine the dataset will be shuffled or not
    // TODO : add more member variables to support the iteration

public:
    DataLoader(Dataset<DType, LType>* ptr_dataset, int batch_size, bool shuffle = true, bool drop_last = false, int seed = -1)
    {
        // TODO implement
        this -> ptr_dataset = ptr_dataset;
        this -> batch_size = batch_size;
        this -> shuffle = shuffle;
        this -> drop_last = drop_last;
        this -> seed = seed;

        int num_sample = this -> ptr_dataset -> len();

        if(this -> batch_size > num_sample)
        {
            this -> total_batch = 0;
        }
        else
        {
            this -> indices = xt::arange(num_sample);
            if(this -> shuffle == true)
            {
                if(this -> seed >= 0)
                {
                    xt::random::seed(this -> seed);
                    xt::random::shuffle(this -> indices);
                }
                else
                {
                    xt::random::shuffle(this -> indices);
                }
            }

            this -> total_batch = num_sample/(this -> batch_size);

            if(this -> total_batch != 0)
            {
                if(this -> drop_last == true)
                {
                    int remove_element;
                    remove_element = num_sample - total_batch * batch_size;
                    this -> indices = xt::view(this -> indices, xt::range(0, num_sample - remove_element));
                }
            }
        }
    }

    virtual ~DataLoader()
    {
        // TODO implement
    }

    Iterator begin()
    {
        return Iterator(this -> ptr_dataset, this -> indices, 0, this -> batch_size, this -> total_batch);
    }

    Iterator end()
    {
        return Iterator(this -> ptr_dataset, this -> indices, this -> total_batch, this -> batch_size, this -> total_batch);
    }

    // TODO implement foreach
    class Iterator
    {
    private:
        Dataset<DType, LType>* ptr_dataset;
        xt::xarray<int> indices;
        int batch_index;
        int batch_size;
        int total_batch;

    public:
    // TODO implement constructor
        Iterator(Dataset<DType, LType>* ptr_dataset, xt::xarray<int> indices, int batch_index, int batch_size, int total_batch)
        {
            this -> ptr_dataset = ptr_dataset;
            this -> batch_size = batch_size;
            this -> indices = indices;
            this -> total_batch = total_batch;
            this -> batch_index = batch_index;
        }

        Iterator& operator=(const Iterator& iterator)
        {
            // TODO implement
            if(this != &iterator)
            {
                this -> ptr_dataset = iterator.ptr_dataset;
                this -> batch_size = iterator.batch_size;
                this -> indices = iterator.indices;
                this -> total_batch = iterator.total_batch;
                this -> batch_index = iterator.batch_index;
            }
            return *this;
        }

        Iterator& operator++()
        {
            // TODO implement
            this -> batch_index++;
            return *this;
        }

        Iterator operator++(int)
        {
            // TODO implement
            Iterator foo = *this;
            this -> batch_index++;
            return foo;
        }

        bool operator!=(const Iterator& other) const
        {
            // TODO implement
            return this -> batch_index != other.batch_index;
        }

        Batch<DType, LType> operator*() const
        {
            // TODO implement
            int begin_index = this -> batch_index * this -> batch_size;
            int end_index = begin_index + this -> batch_size;

            xt::xarray<DType> batch_data;
            xt::xarray<LType> batch_label;

            if(batch_index == total_batch - 1)
            {
                end_index = indices.size();
                xt::svector<unsigned long> fixed_size_data = ptr_dataset -> get_data_shape();
                xt::svector<unsigned long> fixed_size_label = ptr_dataset -> get_label_shape();

                fixed_size_data[0] = end_index - begin_index;
                batch_data = xt::empty<DType>(fixed_size_data);

                fixed_size_label[0] = end_index - begin_index;
                batch_label = xt::empty<LType>(fixed_size_label);
            }
            else
            {
                end_index = begin_index + this -> batch_size;
                xt::svector<unsigned long> fixed_size_data = ptr_dataset -> get_data_shape();
                xt::svector<unsigned long> fixed_size_label = ptr_dataset -> get_label_shape();

                fixed_size_data[0] = batch_size;
                batch_data = xt::empty<DType>(fixed_size_data);

                fixed_size_label[0] = batch_size;
                batch_label = xt::empty<LType>(fixed_size_label);
            }
          
            for(int i = begin_index; i < end_index; i++)
            {
                int idx = this -> indices[i];
                DataLabel<DType, LType> data_label = this -> ptr_dataset -> getitem(idx);
                xt::view(batch_data, i - begin_index, xt::all()) = data_label.getData();
                xt::view(batch_label, i - begin_index, xt::all()) = data_label.getLabel();
            }

            return Batch<DType, LType>(batch_data, batch_label);
        }
    };
};

#endif /* DATALOADER_H */
