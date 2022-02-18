function! Put_argument(template)
	execute 'normal 0d$'
	let @r=a:template
	execute 'normal "rp'
endfunction

function! Simple_query(column, table)
	call fzf#run({
\		'source': "sqlite3 database.sqlite3 'select " . a:column . ' from ' . a:table . ";'",
\		'sink': function('Put_argument'),
\		'window': {'width': 0.9, 'height': 0.6}
\	})
endfunction

:command Application call Simple_query('application', 'application')
:command Type call Simple_query('type', 'issue_type')
:command Person call Simple_query('name', 'person')
:command Relation call Simple_query('relation', 'issue_relation')
:command Ticket call Simple_query('summary', 'issue')
:command Status call Simple_query('name', 'issue_status')
