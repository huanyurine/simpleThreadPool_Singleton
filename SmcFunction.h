#ifndef SmcFunc_H
#define SmcFunc_H

void func(int i) {
	std::cout << "Task " << i << " is running in thread " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "Task " << i << " is done" << std::endl;
}

void mat_func(Eigen::VectorXd* ptr, const Eigen::Ref<const Eigen::MatrixXd>& mat, const Eigen::Ref<const Eigen::VectorXd>& col_v, int i) {
	(*ptr)(i) = (mat.row(i) * col_v * (i + 1))(0);
}

void vec_func(double* ptr, const Eigen::Ref<const Eigen::RowVectorXd>& row_v, const Eigen::Ref<const Eigen::VectorXd>& col_v, int i) {
	*ptr = (row_v * col_v * (i + 1))(0);
}

#endif // !SmcFunc_H
