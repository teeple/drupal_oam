(function($) {
	Drupal.behaviors.system_monitor = {
		attach : function(context, settings) {
			console.log('system_monitor.js loaded');
			$('#system_monitor_cpu').append('CPU Usage');

			/*
			// Load the Visualization API and the piechart package.
			google.load('visualization', '1.0', {'packages':['corechart']});

			// Set a callback to run when the Google Visualization API is loaded.
			google.setOnLoadCallback(drawChart);

			// Callback that creates and populates a data table,
			// instantiates the pie chart, passes in the data and
			// draws it.
			function drawChart() {
			*/

				// Create the data table.
				var data = new google.visualization.DataTable();
				data.addColumn('string', 'Topping');
				data.addColumn('number', 'Slices');
				data.addRows([
					['Mushrooms', 3],
					['Onions', 1],
					['Olives', 1],
					['Zucchini', 1],
					['Pepperoni', 2]
					]);

				// Set chart options
				var options = {'title':'How Much Pizza I Ate Last Night',
					'width':400,
					'height':300};

				// Instantiate and draw our chart, passing in some options.
				var chart = new google.visualization.PieChart(document.getElementById('system_monitor_cpu'));
				chart.draw(data, options);
				/*
			}
			*/

			$('#run-test-ocs-button').bind('click', function(event) {
				$('.run-one-test-case-button').removeClass('fire').addClass( 'fire');
				$('.run-one-test-case-button[order=0]').trigger( 'click');
			});
			$('#add-action-button').bind('click', $.fn.addActionHandler);
			$('.run-one-test-case-button').bind('click', $.fn.runOneTestOcsHandler);
			$('.run-all-test-case-button').bind('click', function(event) {
				var target = $(event.target);
				var nid = $(target).attr('node');
				$('.run-one-test-case-button[node=' + nid +']').removeClass('fire').addClass('fire');
				$('.run-one-test-case-button[node=' + nid +'][tc=0]').trigger('click');
			});
			$('#clear-test-result-button').bind('click', function(event) {
				$('#run-test-ocs-result').text('');
			});

			$('.more_data').bind('click', function(event) {
				var target=$(event.target), 
					content=$(target).parent().children('.content'),
					print=$(target).attr('href'),
					json = $.parseJSON( $(content).text());
				if ( $(content).hasClass('export_result')) {
					json.data = $.parseJSON( json.data);
				}
				$(print).append( prettyPrint( json, {expanded:false}));
			});

			/*
			$('.export_result').each(function(i){
				var json = $.parseJSON($(this).text());
				json.data = $.parseJSON( json.data);
				console.log(json);
				//$(this).append( prettyPrint( json, {expanded:false}));
			});
			var str= $('.export_result').text();
			if ( str != 'undefined') {
				var json = $.parseJSON(str);
				console.log( json);
				$('.export_data').text( prettyPrint( json, {expanded:false}));
			}
			*/
		}
	};

	$.fn.runOneTestOcsHandler = function(event) {
		console.log('run one test case');
		/*
		$('#run-test-ocs-result').append( 
		'<table>' +
		'<thead><tr><th>Response</th><th>Time</th></tr></thead>' +
		'<tbody></tbody>' +
		'</table>'
		);
		*/
		var $target = $(event.target);
		$.fn.runOneTestCase( $target);
	};

	$.fn.runOneTestCase = function( target) {
		console.log( 'target', target);
		$(target).removeClass('fire');

		var nid = $(target).attr('node');
		var order = $(target).attr('order');
		var tc = $(target).attr('tc');
		var title = order + '. ' + $(target).attr('title');
		console.log('test: ', nid);
		/*
		$('.testcase_status[node=' + nid + ']').text('Requesting');
		$('.testcase_check_result[node=' + nid + ']').text( 'RESULT:');
		*/

		var pathname = window.location.pathname.split('/');
		var testsuite = pathname[pathname.length-1];
		console.log('path:', pathname, testsuite);

		// first, get the number of test cases
		var testOcsUrl = '/ajax/test/ocs/{0}/{1}/{2}'.format( testsuite, nid, tc);

		$.ajax({
			url : testOcsUrl,
			type : "get",
			success : function(data) {
				var output = $.parseJSON(data);
				console.log( 'response', output);
			    //$('#run-test-ocs-result').append( '<div>' + JSON.stringify( output, undefined, 2)  + '</div>');
				var config = { expanded : false, maxDepth: 5};

				var d = new Date();
				var tc_idx = output.tc;
				var result = $.parseJSON( output.response.data);
				var test_data = $.parseJSON( output.test_data);
				var test_result = $.parseJSON( output.test_result);
				var counter = [];

				$('#run-test-ocs-result').
					append( '<br>{0}:{1}:{2}<hr style="background-color:darkblue;"><h3>{3}</h3>'.
						format( d.getHours(), d.getMinutes(), d.getSeconds(), title)).
					append( '<h4 id="test_result_{1}_{0}"> TC-{0}    Request {2}</h4>'.format( tc_idx, nid, testOcsUrl)).
					append( '<p> {0}</p>'.format( output.response.request.replace(/\n/g, '<br>')));

				$('.testcase_status[node=' + nid + ']').
					append( '<h4> <a href="#test_result_{1}_{0}">TC-{0}</a> Code: {2}</h4>'.format( tc_idx, nid, output.response.code));

				if ( result != null) {
					var cdr = [];
					if ( typeof( result.cdr) != 'undefined') {
						for( var j in result.cdr) {
							if ( $.isArray( result.cdr[j])) {
								var tmp = [];
								for( var k in result.cdr[j]) {
									tmp[k] = result.cdr[j][k].split(',');
									/*
									var decoded_cdr = $.fn.decodeCDR( cdr[j]);
									counter = decoded_cdr.counters;
									*/
								}
								cdr[j] = tmp;
							}
							else {
								cdr[j] = result.cdr[j];
							}
						}
						result.cdr = cdr;
						console.log( 'cdr', cdr[0][0]);
						var decoded_cdr = $.fn.decodeCDR( cdr[0][0]);
						counter = decoded_cdr.charging_counters; 
						//console.log( 'voice cdr', voice_cdr);
					}

					$('#run-test-ocs-result').
						append( '<br><h4>Result</h4>').
						append( prettyPrint( result, config));

					$('.testcase_status[node=' + nid + ']').
						append( '<p> Result: {0} reason:{1} {2} </p>'.format( 
							(typeof( result.result_code) == 'undefined') ? '??' : result.result_code,
							(typeof( result.result_reason) == 'undefined') ? '??' : result.result_reason,
							(typeof( result.result_desc) == 'undefined') ? '' : '(' + result.result_desc + ')'));
			//	$.fn.runOcsTestCase(nid, data);

					// evalulate condition
					// check result
					var condition = $('.testcase_check[node=' + nid + ']').text();
					var token = condition.match(/[\w\d\.\[\]\']+/g);
					var eval_result = {};
					for( var i in token) {
						try {
							eval_result[token[i]] = eval( token[i]);
						}
						catch( e) {
							eval_result[token[i]] = e;
							eval_result['result'] = 'FAIL';
							break;
						}
					}
					if ( typeof(eval_result['result']) == 'undefined') 
						eval_result['result'] = eval(condition) ? "SUCCESS" : "FAIL";

					console.log( 'check', condition, eval_result);
					console.log( 'counter', counter);
					$('.testcase_check_result[node=' + nid + ']').
						append( '<h4> TC-{0} <a href="#test_result_{1}_{0}">{2}</a></h4>'.
							format( tc_idx, nid, eval_result['result'] ));

					$('#run-test-ocs-result').
						append( '<br><h4> Test Data </h4>').
						append( prettyPrint( eval_result, config));
				}
				else {
					$('#run-test-ocs-result').append( '<br><h4>Result </h4> : {0}<br>'.format( output.response.error));
				}

				// run next test case
				var nextRun = $('.run-one-test-case-button[order={0}][tc={1}]'.format( order, parseInt(tc_idx)+1));
				if ( nextRun.length > 0) {
					if ( $(nextRun).hasClass('fire')) $(nextRun).trigger('click');
				}
				else {
					nextRun = $('.run-one-test-case-button[order={0}][tc=0]'.format( parseInt(order)+1));
					if ( $(nextRun).hasClass('fire')) $(nextRun).trigger('click');
				}
			},
			error : function(data) {
				console.log( 'failed response', data);
				$('#run-test-ocs-result').append( '<br><div id="test_result_' + nid +'"></div>').
					append( '<h3>' + title + '</h3>').
					append( '<h4>Request {0}</h4>'.format( testOcsUrl)).
					append( '<br><h4>Result : Failed</h4>');
			}
		});

	};

	$.fn.runOcsTestCase = function(nid, testData) {
		var testcase = $.parseJSON(testData);
		console.log('test: ', testcase);
		for ( var i = 0; i < testcase.length; i++) {
			var testOcsUrl = '/ajax/test/ocs/' + nid + '/' + testcase[i].nid;
			//            $('#run-test-ocs-result > ul').append( '<li>' + testOcsUrl + '</li>');

			$('.testcase_status[node=' + nid + ']').append(
					'<li node="' + nid + ':' + testcase[i].nid + '"> REQ '
							+ testOcsUrl + '</li>');
			$.ajax({
				url : testOcsUrl,
				type : "get",
				success : function(data) {
					var r = $.parseJSON(data);
					/*
					$('#run-test-ocs-result > ul').append( data);
					 */
					console.log(r);

					$('li[node="' + r.nid + ':' + r.tc_nid + '"]').append(
							'<ul><li> ' + r.nid + ' : ' + r.response.request
									+ '</li>' + '<li> RSP  : '
									+ r.response.code
									+ r.response.status_message + '</li>'
									+ '<li>' + r.response.data + '</li></ul>');

				}
			});
		}
	};
	
	$.fn.addActionHandler = function() {
		var num = $('input[name="num_action"]').val();
		console.log('add action', num);
		$('div.action-' + num).removeClass('hide');
		$('input[name="num_action"]').val( parseInt(num)+1);
	};

	String.prototype.format = function() {
	  var args = arguments;
	    return this.replace(/{(\d+)}/g, function(match, number) { 
			return typeof args[number] != 'undefined'
			  ? args[number]
				: match
								    ;
									  });
	};

	$.fn.decodeCDR = function( fields) {

		// decode used counter
		var cdr = {
			header: fields.slice(0,12),
			scription: fields.slice(12,25),
			charging: fields.slice(25,28),
			service:fields.slice(28),
		}, counter= {}

		console.log( 'cdr', cdr);

		for( var j=0; j< cdr.charging.length; j++){
			if ( cdr.charging[j].length > 0) {
				var used_counter = cdr.charging[j].split('/');
				for( var i=0; i< used_counter.length; i++){
					var usage = used_counter[i].split(':');
					counter[usage[0]] = { usage: parseInt( usage[2]) - parseInt( usage[1])};
				}
			}
		}

		cdr.subscription_key = fields[2];
		cdr.charging_counters = counter;

		console.log( 'decoded cdr', cdr);
		return cdr;
	}

})(jQuery);


