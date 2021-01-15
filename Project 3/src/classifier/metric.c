double accuracy_score(int *y_true, int *y_pred, int n)
{
	int tp = 0, tn = 0, fp = 0, fn = 0;

	for (int i = 0; i < n; ++i) {
		if (y_true[i] == 1) {
			if (y_pred[i] == 1)
				tp++;

			else if (y_pred[i] == 0)
				fn++;
		}
		else if (y_true[i] == 0) {
			if (y_pred[i] == 0)
				tn++;

			else if (y_pred[i] == 1)
				fp++;
		}
	}
	return ((tp + tn + fp + fn) != 0) ? (tp + tn) / (1.0 * (tp + tn + fp + fn)) : 0.0;
}

double precision_score(int *y_true, int *y_pred, int n)
{
	int tp = 0, tn = 0, fp = 0, fn = 0;

	for (int i = 0; i < n; ++i) {
		if (y_true[i] == 1) {
			if (y_pred[i] == 1)
				tp++;

			else if (y_pred[i] == 0)
				fn++;
		}
		else if (y_true[i] == 0) {
			if (y_pred[i] == 0)
				tn++;

			else if (y_pred[i] == 1)
				fp++;
		}
	}
	return ((tp + fp) != 0.0) ? tp / (1.0 * (tp + fp)) : 0.0;
}

double recall_score(int *y_true, int *y_pred, int n)
{
	int tp = 0, tn = 0, fp = 0, fn = 0;

	for (int i = 0; i < n; ++i) {
		if (y_true[i] == 1) {
			if (y_pred[i] == 1)
				tp++;

			else if (y_pred[i] == 0)
				fn++;
		}
		else if (y_true[i] == 0) {
			if (y_pred[i] == 0)
				tn++;

			else if (y_pred[i] == 1)
				fp++;
		}
	}
	return ((tp + fn) != 0) ? tp / (1.0 * (tp + fn)) : 0.0;
}

double f1_score(int *y_true, int *y_pred, int n)
{
	int tp = 0, tn = 0, fp = 0, fn = 0;

	for (int i = 0; i < n; ++i) {
		if (y_true[i] == 1) {
			if (y_pred[i] == 1)
				tp++;

			else if (y_pred[i] == 0)
				fn++;
		}
		else if (y_true[i] == 0) {
			if (y_pred[i] == 0)
				tn++;

			else if (y_pred[i] == 1)
				fp++;
		}
	}
	return ((recall_score(y_true, y_pred, n) + precision_score(y_true, y_pred, n)) != 0)
			   ? 2 * (recall_score(y_true, y_pred, n) * precision_score(y_true, y_pred, n)) /
					 (recall_score(y_true, y_pred, n) + precision_score(y_true, y_pred, n))
			   : 0.0;
}