#include <Eigen/Dense>
#include "ThreadPool.h"
#include "SmcFunction.h"

int main() {
    int mat_row = 2000;
    int mat_col = 3;
    //Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> mat;
    Eigen::MatrixXd mat_in;     mat_in.resize(mat_row, mat_col);    mat_in.setConstant(1);
    Eigen::VectorXd v_in;       v_in.resize(mat_col);               v_in.setConstant(1);
    Eigen::VectorXd v_out;      v_out.resize(mat_row);              v_out.setZero();    //v_out = Eigen::VectorXd::Zero(mat_row);
    
    unsigned int thread_max_num = std::thread::hardware_concurrency() - 1;  //typeid(std::thread::hardware_concurrency()).name()
    ThreadPool& pool = get_thread_pool_instance(thread_max_num);

    std::vector<std::future<double> > results;
    for (int i = 0; i < mat_row; ++i) {
        results.emplace_back(pool.enqueue(vec_func, std::move(mat_in.row(i)), v_in, i));  //mat_in.row(i) 是右值，使用 && 传参；v 是左值，使用 & 传参
    }
    for (int i = 0; i < mat_row; ++i) {
        v_out(i) = results[i].get();
    }
    results.clear();

    std::cout << "----------" << thread_max_num << std::endl;
    std::cout << v_out << std::endl;

    system("pause");
    return 0;
}
