#ifndef SmcFunc_H
#define SmcFunc_H

double vec_func(const Eigen::Ref<const Eigen::RowVectorXd>&& row_v, const Eigen::Ref<const Eigen::VectorXd>& col_v, int i) {
	//std::this_thread::sleep_for(std::chrono::seconds(1));
	return (row_v * col_v * (i + 1))(0);
}

#endif // !SmcFunc_H
