# Events in AbstractTable

Events are used to notify the dependent views that the current table/view is modified. For example a table has these numbers

| x  | y  |
|----|----|
| 0  | 3  |
| 8  | 6  |
| 13 |  0 |
|15  |  2 |
| 18 | 31 |
| 22 | 21 |
| 32 | 30 |
| 64 | 39 |

And a view follows it with this filter **isEven($x)** like below

| x  | y  |
|----|----|
| 0  | 3  |
| 8  | 6  |
| 18 | 31 |
| 22 | 21 |
| 32 | 30 |
| 64 | 39 |

Now if a row {***x =*** 24, ***y =*** 19} is inserted then depending on the filter condition of the view,
view may or may not add this row to the view. In this example as 12 is even so it will be inserted in
the view.

<table>
<tr> <th> Table </th> <th> View </th> </tr>
<tr valign="Top"> <td>

| x  | y  |
|----|----|
| 0  | 3  |
| 8  | 6  |
| 13 | 0  |
| 15 | 2  |
| 18 | 31 |
| 22 | 21 |
| **24** | **19** |
| 32 | 30 |
| 64 | 39 |

</td>
<td>

| x  | y  |
|----|----|
| 0  | 3  |
| 8  | 6  |
| 18 | 31 |
| 22 | 21 |
| **24** | **19** |
| 32 | 30 |
| 64 | 39 |

</td>
</tr>
</table>

A table/view can be modified for different reasons. Some of the common reasons are

- **A new row is inserted**

If a row is inserted then depending on the filter condition it can add or remove
that row in the view. Also as Views keep only indices it needs to update existing indices
according to the index of the inserted row.

- **An existing row is removed**

If a row is removed then if it existed in the view, it will be removed. Indices will be updated
depending on the index of the removed row.

- **A cell value is changed**

If a cell value is changed then there can be many cases, the column might be part of filter
formula. If it is, then view will be needed to filter this row again. This row might be part of the
view. The column might be the key column (sorting column) for the view. Depending on all these conditions
it may need to *add*/*remove*/*move*/*ignore* the row.

- **View is sorted with different column**

If a view's sorting column or order is changed then it needs to notify the dependent views so that they
can refresh the indices.
