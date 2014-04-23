#!/usr/bin/perl

use Modern::Perl;
use IO::File;
use File::Spec::Functions qw/rel2abs catfile catdir/;
use Parse::RecDescent;
use Data::Dump qw/dump/;
use Getopt::Long;

$::RD_ERRORS = 1; #Parser dies when it encounters an error
$::RD_WARN   = 1; # Enable warnings- warn on unused rules &c.
#$::RD_HINT   = 1; # Give out hints to help fix problems.
$::RD_AUTOACTION = q { $item[1] };

our %macro;
our @LOCAL_INC_PATH;
our @ENV_PATH = split /[:;]+/, $ENV{'PATH'};
our @INCLUDE_SEARCH;
our $BUILTIN_MACROS = '';

our $LANG = '';

our %opts = (
   'include' => [],
   'define' => {},
);

GetOptions(
   \%opts,
   "include|I=s",
   "define|D=s"
) or die ("error in command line arguments\n");

say dump(%opts);

our $file_path = $ARGV[0] // 'test.h';

our $pp_parser = make_preprocessor_parser();

$LANG = detect_lang($file_path);
@INCLUDE_SEARCH = find_inc_search($LANG);

#say "BM: ".$BUILTIN_MACROS;
my $macros = $pp_parser->doc($BUILTIN_MACROS);
for my $item (@$macros)
{
	if (scalar @$item == 2)
	{
		$macro{$item->[1]} = '';
		#say "define ".$item->[1];
	}
	elsif (scalar @$item == 3)
	{
		$macro{$item->[1]} = $item->[2];
		#say "define ".$item->[1].' = '.$item->[2];
	}
	else
	{
		$macro{$item->[1]} = [ $item->[2], $item->[3] ];
		#say "define ".$item->[1].' '.dump($item->[2]).' '.$item->[3];
	}
}
#say 'builtins: '.dump($macros);

#exit;
#say dump %macro;

$::RD_TRACE = 1;
if(!pre_process($ARGV[0] // 'test.h', $LANG))
{
	warn("damn.");
}

say dump \%macro;

sub macro {
	my ($key) = @_;
	my $val;
	
	if (!exists $macro{$key})
	{
		say "macro: $key not found";
		return;
	}
	
	if(ref($macro{$key}) eq 'ARRAY')
	{
		$val = $macro{$key}->[1];
	}
	else
	{
		$val = $macro{$key};
	}
	
	say "macro: $key = ".$val;
	
	return $val;
}

sub pre_process {
	my ($fpath, $lang) = @_;
	
	my $fh = IO::File->new($fpath);
	if (!defined $fh)
	{
		warn("failed to open $fpath: $!");
		return;
	}
	
	my $text = '';
	{ local $/; $text = <$fh>; }
	
	$text =~ s/\\[\r\n]//mg;
	
	#say "text: $text";
	my $tree = $pp_parser->doc($text);
	
	#say "tree: ".dump($tree);
	
	my @kept;
	my @keep = (1);
	my $keep_depth = 1;
	state $last_file;
	state @LAST_PATH;
	
	for my $item (@$tree)
	{
		if($keep[-1])
		{
			#say "keep: '".$item->[0]."'" if $item->[0] ne 'text';
			if($item->[0] eq 'include')
			{
				my $file = $item->[1];
				say "inc_raw: ".$file;
				if ($file =~ /^\w+$/)
				{
					# macro!
					
					say "inc macro: ".dump($macro{$file});
					$file = macro($file);
				}
				
				my $sys_first =  substr($file, 0, 1) eq '<'; # system first
				
				$file =~ s/[<"]([^<"]+)[>"]/$1/;
				
				if ($sys_first)
				{
					@LAST_PATH = (@INCLUDE_SEARCH, @{$opts{'include'}});
					$file = which($file, @LAST_PATH);
				}
				else
				{
					@LAST_PATH = (@{$opts{'include'}}, @INCLUDE_SEARCH);
					$file = which($file, @LAST_PATH);
				}

				say "inc: ".$file;
				$last_file = $file;
				
				my $sub_tree = pre_process($file, $lang);
				if(!defined $sub_tree)
				{
					return;
				}
			}
			elsif($item->[0] eq 'include_next')
			{
				my $file = $item->[1];
				say "inc_next_raw: ".$file;

				if ($file =~ /^\w+$/)
				{
					# macro!
					
					say "inc macro: ".dump($macro{$file});
					$file = macro($file);
				}
				
				$file =~ s/[<"]([^<"]+)[>"]/$1/;
				
				my $found_idx = 0;

				for (my $i = 0; $i < scalar(@LAST_PATH)-1; $i++)
				{
					my $search = $LAST_PATH[$i];
					say "inc_next look: ".$search.' <> '.$last_file;
					if($last_file =~ m|^$search(.*)$|)
					{
						say "inc_next found: ".$search;
						$found_idx = $i;
						#$found_file = catfile($base, $file);
						last;
					}
				}
				
				my @SUB_PATH = @LAST_PATH[$found_idx+1 .. $#LAST_PATH];
				
				my $found_file = which($file, @SUB_PATH);
				
				say "inc_next: ".$found_file;
				
				my $sub_tree = pre_process($found_file, $lang);
				if(!defined $sub_tree)
				{
					return;
				}
			}
			elsif($item->[0] eq 'define')
			{
				#macro
				
				if (scalar @$item == 2)
				{
					$macro{$item->[1]} = '';
					#say "define ".$item->[1];
				}
				elsif (scalar @$item == 3)
				{
					$macro{$item->[1]} = $item->[2];
					#say "define ".$item->[1].' = '.$item->[2];
				}
				else
				{
					$macro{$item->[1]} = [ $item->[2], $item->[3] ];
					#say "define ".$item->[1].' '.dump($item->[2]).' '.$item->[3];
				}
				
				if($item->[1] =~ /^ALLEGRO_/)
				{
					say 'define: '.$item->[1];
				}
			}
			elsif($item->[0] eq 'undef')
			{
				if(!exists $macro{$item->[1]})
				{
					#say "WARN: trying to undef undefined macro ".$item->[1];
				}
				else
				{
					delete $macro{$item->[1]};
				}
			}
			elsif($item->[0] eq 'ifdef')
			{
				if(exists $macro{$item->[1]})
				{
					#say "macro $item->[1] exists";
					
					$keep_depth++;
					push @keep, $keep_depth;
				}
				else
				{
					$keep_depth++;
					push @keep, 0;
				}
			}
			elsif($item->[0] eq 'ifndef')
			{
				if(!exists $macro{$item->[1]})
				{
					#say "macro $item->[1] doesn't exist";
					$keep_depth++;
					push @keep, $keep_depth;
				}
				else
				{
					$keep_depth++;
					push @keep, 0;
				}
			}
			elsif($item->[0] eq 'if')
			{
				# fun part

				$keep_depth++;
				
				my $res = eval_if($item->[1]) ? $keep_depth : 0;
				
				push @keep, $res;
			}
			elsif($item->[0] eq 'elif')
			{
				# fun part
				say "elif: ".dump $item->[1];
				#$keep_depth++;
				
				my $res = eval_if($item->[1]) ? $keep_depth : 0;
				
				my $old_depth = pop @keep;
				push @keep, !$old_depth ? $res : 0;
			}
			elsif($item->[0] eq 'text')
			{
				#push @kept, $item->[1] if $keep;
			}
			elsif($item->[0] eq 'endif')
			{
				$keep_depth = pop @keep;
			}
			elsif($item->[0] eq 'else')
			{
				my $old_depth = pop @keep;
				push @keep, !$old_depth ? $keep_depth : 0;
			}
			elsif($item->[0] eq 'error')
			{
				die('ERROR: '.$item->[1]);
			}
			elsif($item->[0] eq 'warning')
			{
				warn('WARN: '.$item->[1]);
			}
			else
			{
				warn('UNSP: unkown directive: '.dump($item));
			}
		}
		#elsif($item->[0] eq 'endif')
		#{
		#	$keep_depth = pop @keep;
		#}
		elsif($item->[0] =~ /^if\w*/)
		{
			$keep_depth++;
			push @keep, 0;
		}
		elsif($item->[0] eq 'error')
		{
			say('found error: '.$item->[1]);
		}
		elsif($item->[0] eq 'warning')
		{
			say('found warning: '.$item->[1]);
		}
		elsif($item->[0] eq 'text') {}
		else
		{
			#say 'ign: '.$item->[0];
			#say "keeps: ".join(',',@keep);
			#say "keep: ".$keep[-1]." == $keep_depth";
			if($item->[0] eq 'endif')
			{
				$keep_depth = pop @keep;
				if($keep[-1] == $keep_depth)
				{
					#say "wat";
				}
			}
			#elsif($item->[0] eq 'else')
			#{
			#	my $old_depth = pop @keep;
			#	push @keep, !$old_depth ? $keep_depth : 0;
			#	#push @keep, $keep_depth;
			#}
			
			#say "ign: ".$item->[0];
		}
	}
	
	return 1;
}

sub detect_lang {
	my ($fpath) = @_;

	my $fh = IO::File->new($fpath) or die ("failed to open $fpath: $!");
	my $text = '';
	{ local $/; $text = <$fh>; }

	my $cxx_score = 0;
	my $cxx_thresh = 100;
	
	# stupid heuristics, [2] means this type of pattern can be checked multiple times
	# [3] stores the number of times a pattern has been matched.
	my @cxx_pats = (
		[ 5, qr|#\s*include\s*<\w+>|, 1, 0 ], # no EXT
		[ 1, qr|\b\w+::\w+\b|, 1, 0 ],
		[ 20, qr|\bclass\s+\w+;|, 1, 0 ],
		[ 20, qr|\bclass\s+\w+\s+(?:.*{?)$|, 1, 0 ],
		[ 20, qr|\btemplate\s*<.*>|, 1, 0 ],
		[ 10, qr|\bnew\s+\w+(?:\s*\(.*\))?|, 1, 0 ],
		[ 1, qr|\bextern\s*"C"|, 1, 0 ],
		[ 1, qr|\bbool\s+|, 1, 0 ]
	);
	
	$text =~ s|/\*.*?\*/||s;

	my $found_cxx = 0;
	my @lines = split /[\r\n]+/, $text;
	for my $pat (@cxx_pats)
	{
		for my $line (@lines)
		{
			$line =~ s|//.*$||;
			
			if (($pat->[2] || (!$pat->[2] && !$pat->[3])) && $line =~ $pat->[1])
			{
				$pat->[3]++;
				
				$cxx_score += $pat->[0];
				#say "found cxx pat ($pat->[1]) score(".($cxx_score/$cxx_thresh)."): $line";
				$found_cxx = 1 if $cxx_score >= $cxx_thresh;
			}
		}
	}
	
	if ($found_cxx)
	{
		say "found C++ with a likeleyhood of ".$cxx_score/$cxx_thresh;
	}
	
	#say dump(@cxx_pats);
	return $found_cxx ? 'CPP' : 'C';
}

sub find_include {
	my ($lang, $fpath, @inc_path) = @_;
	return which($fpath, @INCLUDE_SEARCH, @inc_path);
}


sub find_inc_search_gcc {
	my $lang = uc(shift // 'C');
	my $prog = 'cc1';
	
	if ($lang =~ /C(?:\+\+|PP)/)
	{
		$prog = 'cc1plus';
	}
	
	my $gcc = which('gcc');
	
	my $target = `$gcc -dumpmachine 2>&1`;
	chomp $target;

	say "target: ".$target;
	
	#say "running: $gcc -print-prog-name=$prog";
	my $prog_path = `$gcc -print-prog-name=$prog 2>&1`;
	chomp $prog_path;
	
	#say "prog_path: $prog_path";
	my $text = `echo | $prog_path -imultiarch $target -dM -E -v 2>&1`;
	
	# #include <...> search starts here:
	#  /usr/include/c++/4.8
	#  /usr/include/x86_64-linux-gnu/c++/4.8
	#  /usr/include/c++/4.8/backward
	#  /usr/lib/gcc/x86_64-linux-gnu/4.8/include
	#  /usr/local/include
	#  /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed
	#  /usr/include
	# End of search list.

	my @search;
	my $in_search_list = 0;
	for my $line (split /[\r\n]+/, $text)
	{
		chomp $line;
		if ($line =~ /^\s*#include\s*<\.\.\.> search starts here:/)
		{
			$in_search_list = 1;
		}
		elsif ($in_search_list && $line =~ /^\s*End of search list./)
		{
			$in_search_list = 0;
		}
		elsif ($in_search_list)
		{
			my ($path) = ($line =~ /^\s*(.*)\s*$/);
			#say "inc: ".$path;
			push @search, $path;
		}
		elsif ($line =~ /^#define\s+\w+\s+.*$/)
		{
			$BUILTIN_MACROS .= $line . "\n";
		}
		#else
		#{
		#	say "wut: $line";
		#}
	}
	
	return @search;
}

sub find_inc_search {
	return find_inc_search_gcc($_[0]);
}

sub which {
	my ($name, @SEARCH) = @_;
	
	my $find_one = !wantarray();
	return if !defined $find_one; # void context;
	
	# setup search to default to PATH if the search paths aren't passed in
	@SEARCH = @ENV_PATH unless scalar @SEARCH;
	
	my @found;
	
	for my $search (@SEARCH)
	{
		$search = rel2abs($search);
		#say "searching: $search";
		my $path = catfile($search, $name);
		if (-e $path && -f $path)
		{
			#say "found: ".$path;
			return $path if $find_one;
			push @found, $path;
		}
	}
	
	if ($find_one)
	{
		return $name;
	}
	
	return @found;
}

sub eval_if {
	my $tree = $_[0];
	my $ret = 0;
	
	my $not = 0;
	my @stack = ($tree);
	
	say "eval_if: ".dump($tree);
	while(defined(my $item = pop @stack))
	{
		#last unless $item;
		my $ref = ref($item) eq 'ARRAY';
	
		#say 'item: '.dump($item);
		if($ref)
		{
			my $node = $item;
			my $child_ref = ref($node->[1]) eq 'ARRAY';
			
			#say 'node: '.dump($tree) unless defined $node->[0];
			
			#say 'st: '.dump(@stack);
			#if($child_ref)
			#{
			#	push @stack, $node;
			#}
			#	#say "push node: ".dump($node->[1]);
			#	my $cnt = scalar @$node;
			#	say "pushing ".($cnt-1)." items";
			#	push @stack, $node;
			#	push @stack, @{$node}[1 .. $cnt-1];
			#	#say "push stack: ".dump(@stack);
			#	
			#	next;
			#}
			
			my $op = $node->[0];
			say "op: ".$op;
			
			if($op eq 'identifier')
			{
				my $val = macro($node->[1]) // 0;
				
				#if(scalar @$val)
#				{
#					say "macro: ".dump($val);
#					$val = $val->[1];
#				}
				
				#say 'id: '.dump($val);
				$node->[1] = undef;
				unshift @stack, $val;
			}
			elsif($op eq 'neg')
			{
				if($child_ref)
				{
					push @stack, $node;
					push @stack, $node->[1];
					$node->[1] = undef;
				}
				else
				{
					my $val = -(shift @stack);
					#say 'neg: '.dump($val);

					unshift @stack, $val;
				}
			}
			elsif($op eq 'not')
			{
				if($child_ref)
				{
					#say "push not";
					push @stack, $node;
					push @stack, $node->[1];
					$node->[1] = undef;
				}
				else
				{
					my $val = shift(@stack) ? 0 : 1;
					#say 'not: '.$val;

					unshift @stack, $val;
					#next;
				}
			}
			elsif($op eq 'defined')
			{
				my $def = exists($macro{$node->[1]}) || 0;
				$node->[1] = undef;
				#say 'def: '.$def;
				unshift @stack, $def;
				#next;
			}
			elsif($op eq '==')
			{
				if(ref($node->[1]) eq 'ARRAY' || ref($node->[2]) eq 'ARRAY')
				{
					push @stack, $node;
					if(ref($node->[1]) eq 'ARRAY')
					{
						push @stack, $node->[1];
						$node->[1] = undef;
					}
					if(ref($node->[2]) eq 'ARRAY')
					{
						push @stack, $node->[2];
						$node->[2] = undef;
					}
				}
				else
				{
					#say "and stack: ".dump(@stack);
					my ($v1, $v2) = (shift @stack, shift @stack);
					my $val = int($v1 == $v2);
					#say "and: ($v1 && $v2) ".$val;
					unshift @stack, $val;
				}
			}
			elsif($op eq '!=')
			{
				if(ref($node->[1]) eq 'ARRAY' || ref($node->[2]) eq 'ARRAY')
				{
					push @stack, $node;
					if(ref($node->[1]) eq 'ARRAY')
					{
						push @stack, $node->[1];
						$node->[1] = undef;
					}
					if(ref($node->[2]) eq 'ARRAY')
					{
						push @stack, $node->[2];
						$node->[2] = undef;
					}
				}
				else				
				{
					#say "and stack: ".dump(@stack);
					my ($v1, $v2) = (shift @stack, shift @stack);
					my $val = int($v1 != $v2);
					#say "and: ($v1 && $v2) ".$val;
					unshift @stack, $val;
				}
			}
			elsif($op eq 'and')
			{
				if(ref($node->[1]) eq 'ARRAY')
				{
					push @stack, $node;
					push @stack, $node->[1];
					$node->[1] = undef;
				}
				elsif(ref($node->[2]) eq 'ARRAY')
				{
					my $val = $stack[0];
					if(!defined $val || !$val)
					{
						#say "sand: ".$val.' | '.dump(@stack);
					}
					else
					{
						push @stack, $node;
						push @stack, $node->[2];
						$node->[2] = undef;
					}
				}
				else
				{
					#say "and stack: ".dump(@stack);
					my ($v1, $v2) = (shift @stack, shift @stack);
					my $val = int($v1 && $v2);
					say "and: ($v1 && $v2) ".$val;
					unshift @stack, $val;
				}
			}
			elsif($op eq 'or')
			{
				if(ref($node->[1]) eq 'ARRAY')
				{
					push @stack, $node;
					push @stack, $node->[1];
					$node->[1] = undef;
				}
				elsif(ref($node->[2]) eq 'ARRAY')
				{
					my $val = $stack[0];
					if(defined $val && $val)
					{
						#say "sor: ".$val;
					}
					else
					{
						push @stack, $node;
						push @stack, $node->[2];
						$node->[2] = undef;
					}
				}
				else
				{
					my ($v1, $v2) = (shift @stack, shift @stack);
					my $val = int($v1 || $v2);
					say "or: ($v1 || $v2) ".$val;
					unshift @stack, $val;
				}
			}
			elsif($item->[0] eq 'mcall')
			{
				my ($n, $mname, @marg_values) = @{$item};
				if(exists $macro{$mname})
				{
					my ($marg_names, $macro_text) = @{$macro{$mname}};
					my @args = map { shift @stack } @marg_values;
					say "mcall: $mname ".dump($macro{$mname});
				}
				else
				{
					warn "attempt to call undefined macro $mname";
				}
				#my $val = mcall_expand($mname, $marg_names,  $macro_text
			}
			else
			{
				say "unode: ".dump($node);
			}
			#say 'stack: '.dump(@stack);
			#pop $stack[-1]
			#if(ref $stack[-1] && ref($stack[-1][1]) eq 'ARRAY') { say 'dp: '.dump pop @{$stack[-1][1]} }
		}
		else
		{
			say "lit: ".$item;
			$ret = $item;
		#	say "lit?";
		}
	}
	
	#say dump(@stack);
	return $ret;
}

sub make_preprocessor_parser
{
	$Parse::RecDescent::skip = qr/(\\[\r\n]+|[ \t]*)/;
#<skip: qr{\s* (/[*] .*? [*]/ \s*)*}x>	
	my $grammar = <<'EOG';

integer: /\-?[0-9]+L?/ { $return = $item[1] }
float: /\-?[0-9]+\.[0-9]+F?/ { $return = $item[1] }
number: float | integer
string: '"' /[^"]+/ '"' { $return = $item[2] }
identifier: /[a-zA-Z_]+[a-zA-Z0-9_]*/ { $return = $item[1] }

newline: /[\r\n]+/
newline_nbs: /(?<!\\)[\r\n]+/

ml_value: '\\' newline /.+/ { $return = $item[3] }
	| # nothing
	
value: /[^\r\n]+(?=\\)/ ml_value { $return = $item[1].' '.$item[2] }
	| /[^\r\n]+/
	
macrodef : 'define' /[a-z_]\w*/i
	'(' <leftop: /[a-z]+/i ',' /[a-z]+/i> ')' compound
		{ $return =  [ 'define', $item[2], $item[4], $item[-1] ] }
	
	| 'define' /[a-z_]\w*/i compound
		{ $return = [ 'define', $item[2], $item[3] ] }
		
	| 'define' /[a-z_]\w*/i 
		{ $return = [ 'define', $item[2] ] }

lbinops: '==' | '>=' | '<=' | '>' | '<' | '!='
nbinops: '+' | '-' | '/' | '*'

if_expr: if_or_expr { print("expr: ".((ref($item[1]) eq 'ARRAY') ? ::dump(@{$item[1]}) : $item[1])."\n"); $return = $item[1]; }

if_or_expr: if_and_expr '||' if_expr { $return = [ 'or', $item[1], $item[3] ] }
	| if_and_expr

if_and_expr: if_cmp_expr '&&' if_expr { $return = [ 'and', $item[1], $item[3] ] }
	| if_cmp_expr

if_cmp_expr: if_arith_expr lbinops if_cmp_expr { $return = [ $item[2], $item[1], $item[3] ] }
	| if_arith_expr

#sum   : term sum_[ $item[1] ]
#sum_  : /[+-]/ term sum_[ [ $item[1], $arg[0], $item[2] ] ]
#	| { $arg[0] }
         
if_arith_expr: if_not_expr nbinops if_arith_expr { $return = [ $item[2], $item[1], $item[3] ] }
	| if_not_expr

#if_and_expr:  if_cmp_expr '&&' if_and_expr { $return = [ 'and', $item[1], $item[3] ] }
#	| if_cmp_expr

#if_cmp_expr: if_cmp2_expr '==' if_cmp_expr { $return = [ 'eq', $item[1], $item[3] ] }
#	| if_cmp2_expr '!=' if_expr { $return = [ 'ne', $item[1], $item[3] ] }
#	| if_cmp2_expr

#if_cmp2_expr: if_not_expr '<=' if_cmp2_expr { $return = [ 'le', $item[1], $item[3] ] }
#	| if_not_expr '>=' if_expr { $return = [ 'ge', $item[1], $item[3] ] }
#	| if_not_expr
	
if_not_expr: '!' if_unary_expr { $return = [ 'not', $item[2] ] }
	| if_unary_expr

if_unary_expr: '-' if_brack_expr { $return = [ 'neg', $item[2] ] }
	| '+' if_brack_expr { $return = $item[2] }
	| if_brack_expr

	
if_brack_expr: '(' if_expr ')' { $return = $item[2] }
	| if_defined_expr

if_defined_expr: 'defined' '(' identifier ')' { $return = [ 'defined', $item[3] ] }
	| 'defined' identifier { $return = [ 'defined', $item[2] ] }
	| if_mcall_expr

if_mcall_expr: identifier '(' <leftop: if_expr ',' if_expr > ')' { $return = [ 'mcall', $item[1], @{$item[3]} ] }
	| if_lit
	
if_lit: number | string | identifier { $return = [ 'identifier', $item[1] ] } | if_charlit

if_charlit: '\'' /\w/ '\'' { $return = $item[2] }
	| <error: invalid character literal>

error_directive: 'error' /.*/ { $return = [ 'error', $item[2] ] }
warn_directive: 'warning' /.+/ { $return = [ 'warning', $item[2] ] }
if_directive: /if\b/ if_expr { $return = [ 'if', $item[2] ] }
else_directive: 'else' /.+/ { $return = [ 'else', $item[2] ] }
	| 'else' { $return = [ 'else' ] }
ifdef_directive: 'ifdef' value { $return = [ 'if', ['defined', $item[2] ] ] }
ifndef_directive: 'ifndef' value { $return = [ 'if', ['not', ['defined', $item[2] ] ] ] }
elif_directive: 'elif' if_expr { $return = [ 'elif', $item[2] ] }

include_directive_file: '<' /[^>]+/ '>' { $return = [ 'include', '<'.$item[2].'>' ] }
	| '"' /[^"]+/ '"' { $return = [ 'include', '"'.$item[2].'"' ] }
	| identifier { $return = [ 'include', $item[1] ] }
	
include_directive: 'include' include_directive_file { $return = $item[-1]; }
include_next_directive: 'include_next' include_directive_file { $return = [ 'include_next', $item[-1] ] } 

directives: if_directive
	| ifndef_directive
	| include_directive
	| macrodef
	| else_directive
	| elif_directive
	| ifdef_directive
	| error_directive
	| warn_directive
	| include_next_directive
	| identifier value { $return = [ $item[1], $item[2] ] }
	| identifier { $return = [ $item[1] ] }
	| <error: die>
	
directive: '#' directives { $return = $item[-1] }

text: /.+/ { $return = [ 'text', $item[1] ] }

comment_end: '*/'
comment_start: '/*'

comment_line: <skip: qr/[ \r\n\t]*/> m{.+} ...comment_end { $return = $item[-1] }
	| <skip: qr/[ \r\n\t]*/> m{.+} { $return = $item[-1] }
	
comment_cont: <skip: qr/[ \r\n\t]*/> comment_line(s) { CORE::say('c: '.$item[-1]); $return = $item[-1] }
	| comment_line comment_end { $return = $item[1] } 
	
comment: <skip: qr/[ \r\n\t]*/> comment_start comment_cont comment_end { $return = join('', @{$item[-2]}); }

statement: directive | text

compound: statement

doc: compound(s? /[\r\n]+/)

	
EOG
	say 'grammar: '.$grammar;
# <skip: qr/[ \t]*/> directive '\\' newline /.*/ { $return = [ 'stmt', $item[1], $item[3], $item[4] ] }
	return new Parse::RecDescent ($grammar) or die "Bad grammar!\n";
	
}
