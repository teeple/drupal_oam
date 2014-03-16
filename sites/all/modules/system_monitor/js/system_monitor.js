(function($) {
	// define ajax chart
	function AjaxChart(id, cols, vmax) {
		// assign id, cols
		this.id = id;
		this.cols = cols;
		this.max = this.index = 100;

		// set initial value
		this.data = new google.visualization.DataTable();
		this.data.addColumn('number', 'index');
		this.initval = [];
		for ( var i=0; i < cols.length; i++) {
			this.initval.push(0);
			this.data.addColumn('number', cols[i]);
		}

		// set view max
		this.vmax = vmax;

		// get jquery object
		var obj = $('#' + id);
		this.interval = $(obj).attr('interval');

		// build data
		for ( var i = 0; i < this.max; i++) {
			this.data.addRow([ i ].concat(this.initval));
		}

		// build options
		this.options = {
			width : $(obj).attr('width'),
			height : $(obj).attr('height'),
		}
		if (vmax > 0) {
			this.options['vAxis'] = {
				viewWindowMode : 'explicit',
				viewWindow : {
					max : vmax,
					min : 0,
				}
			}
		}

		// draw initial chart
		this.chart = new google.visualization.LineChart(document
				.getElementById(id));
	}

	// method : draw
	AjaxChart.prototype.draw = function() {
		this.chart.draw(this.data, this.options);
		return this;
	}

	// method : add data
	AjaxChart.prototype.addData = function(value) {
		this.data.removeRow(0);
		this.data.addRow([ this.index++ ].concat(value));
		return this;
	}

	Drupal.behaviors.system_monitor = {
		attach : function(context, settings) {
			console.log('system_monitor.js loaded');

			// Callback that creates and populates a data table,
			// instantiates the pie chart, passes in the data and
			// draws it.
			function drawChart() {
				console.log('draw chart');
				var interval = 3;
				var chartList = {
					'cpu_load' : new AjaxChart('system_monitor_cpu_load', [
							'usr', 'nice', 'sys', 'load' ], 105).draw(),
					'cpu_loadavg' : new AjaxChart('system_monitor_cpu_loadavg',
							[ '1min', '5min', '15min' ], 0).draw(),
				};

				function tick() {

					var req = $.ajax({
						type : "GET",
						url : "/drupal_oam/ajax/mon/cpu_load",
						dataType : "json",
						complete : function(msg) {
							var rsp = $.parseJSON(msg.response);
							for ( var i in rsp.data) {
								var c = chartList[i];
								c.addData(rsp.data[i].value);
								c.draw();
							}
						}
					});
				}
				setInterval(tick, interval * 1000);
			}

			// Load the Visualization API and the chart package.
			google.load("visualization", "1", {
				packages : [ "corechart" ],
				"callback" : drawChart
			});
			console.log('google api loaded');

		}
	};

})(jQuery);
