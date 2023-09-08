#include <chrono>
#include <Eigen/Dense>
#include "ThreadPool.h"
#include "SmcFunction.h"

void save() {
    ThreadPool& pool = get_instance(std::thread::hardware_concurrency());
    ThreadPool& pool_a = get_instance(std::thread::hardware_concurrency());
    for (int i = 0; i < 4; ++i) {
        pool.get_id();
        pool.enqueue(func, i);
        pool_a.enqueue(func, 4 + i);
    }
}

int main() {
    //int mat_row = 2000000;
    int mat_row = 20;
    int mat_col = 3;
    Eigen::MatrixXd mat_in; //Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> mat;
    Eigen::VectorXd mat_out, v;
    mat_in.resize(mat_row, mat_col);
    mat_out.resize(mat_row);
    v.resize(mat_col);
    mat_in.setConstant(1);
    mat_out.setZero();  //mat_out = Eigen::VectorXd::Zero(mat_row);
    v.setConstant(1);
    unsigned int thread_max_num = std::thread::hardware_concurrency();  //typeid(std::thread::hardware_concurrency()).name()
    ThreadPool& pool = get_instance(thread_max_num);

    //Eigen::VectorXd* ptr = &mat_out;
    //std::vector<std::future<void>> results;
    //for (int i = 0; i < mat_row; ++i) {
    //    results.emplace_back(pool.enqueue(mat_func, ptr, mat_in, v, i));
    //}

    double* ptr = nullptr;
    std::vector<std::future<void>> results;
    for (int i = 0; i < mat_row; ++i) {
        ptr = &(mat_out(i));
        results.emplace_back(pool.enqueue(vec_func, ptr, std::move(mat_in.row(i)), v, i));  //mat_in.row(i) 是右值，使用 && 传参；v 是左值，使用 & 传参
    }

    for (auto&& result : results) {
        result.get();
    }
    ptr = nullptr;
    delete ptr;

    std::cout << "----------" << thread_max_num << std::endl;
    std::cout << mat_out << std::endl;
    //system("pause");
    return 0;
}
